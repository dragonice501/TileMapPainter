#pragma once

#include <stdint.h>
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <iostream>

class Entity
{
public:
	Entity(uint32_t id) : id(id) {};
	Entity(const Entity& entity) = default;

	uint32_t GetId() const { return id; }

	Entity& operator=(const Entity& other) = default;
	bool operator==(const Entity& other) { return id == other.id; }
	bool operator >(const Entity& other) const { return id > other.id; }
	bool operator <(const Entity& other) const { return id < other.id; }

	template<typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
	template<typename TComponent> void RemoveComponent();
	template<typename TComponent> bool HasComponent() const;
	template<typename TComponent> TComponent& GetComponent() const;

	class Registry* registry;

private:
	uint32_t id;
};

const uint32_t MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent
{
protected:
	static uint32_t nextId;
};

template <typename T>
class Component: public IComponent
{
public:
	static uint32_t GetId()
	{
		static auto id = nextId++;
		return id;
	}
};

class System
{
public:
	System() = default;
	~System() = default;

	void AddEntityToSystem(Entity entity);
	void RemoveEntityFromSytem(Entity entity);
	const std::vector<Entity>& GetSystemEntities() const { return entities; }
	const Signature& GetComponentSignature() const { return componentSignature; }

	template <typename TComponent> void RequireComponent();

private:
	Signature componentSignature;
	std::vector<Entity> entities;
};

class IPool
{
public:
	virtual ~IPool(){}
};

template <typename T>
class Pool: public IPool
{
public:
	Pool(uint32_t size = 100)
	{
		data.resize(size);
	}
	virtual ~Pool() override = default;

	bool IsEmpty() { return data.empty(); }
	uint32_t GetSize() const { return data.size(); }
	void Resize(uint32_t n) { data.resize(n); }
	void Clear() { data.clear(); }
	void Add(T object) { data.push_back(object); }
	void Set(uint32_t index, T object) { data[index] = object; }
	T& Get(uint32_t index) { return static_cast<T&>(data[index]); }
	T& operator[](uint32_t index) { return data[index]; }

private:
	std::vector<T> data;
};

class Registry
{
public:
	Registry() = default;

	void Update();

	Entity CreateEntity();
	void AddEntityToSystems(Entity entity);

	template<typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template<typename TComponent> void RemoveComponent(Entity entity);
	template<typename TComponent> bool HasComponent(Entity entity);
	template<typename TComponent> TComponent& GetComponent(Entity entity) const;

	template<typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
	template<typename TSystem> void RemoveSystem(System system);
	template<typename TSystem> void HasSystem(System system);
	template<typename TSystem> TSystem& GetSystem() const;

private:
		uint32_t numEntites = 0;

		std::vector<std::shared_ptr<IPool>> componentPools;
		std::vector<Signature> entityComponentSignatures;
		std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

		std::set<Entity> entitiesToBeAdded;
		std::set<Entity> entitiesToBeKilled;
};

template<typename TComponent, typename ...TArgs>
inline void Entity::AddComponent(TArgs && ...args)
{
	registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template<typename TComponent>
inline void Entity::RemoveComponent()
{
	registry->RemoveComponent<TComponent>(*this);
}

template<typename TComponent>
inline bool Entity::HasComponent() const
{
	return registry->HasComponent<TComponent>(*this);
}

template<typename TComponent>
inline TComponent& Entity::GetComponent() const
{
	return registry->GetComponent<TComponent>(*this);
}

template<typename TComponent>
inline void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}

template<typename TComponent, typename ...TArgs>
inline void Registry::AddComponent(Entity entity, TArgs && ...args)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	if (componentId >= componentPools.size())
	{
		componentPools.resize(componentId + 1, nullptr);
	}

	if (componentPools[componentId] == nullptr)
	{
		
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		componentPools[componentId] = newComponentPool;
	}

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	if (entityId >= componentPool->GetSize())
	{
		componentPool->Resize(numEntites);
	}

	TComponent newComponent(std::forward<TArgs>(args)...);

	componentPool->Set(entityId, newComponent);
	entityComponentSignatures[entityId].set(componentId);

	//std::cout << "Component id " << static_cast<int>(componentId) << " was added to entity " << entityId << std::endl;
}

template<typename TComponent>
inline void Registry::RemoveComponent(Entity entity)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	std::cout << "Component " << static_cast<int>(componentId) << " was removed from entity " << static_cast<int>(entityId);

	entityComponentSignatures[entityId].set(componentId, false);
}

template<typename TComponent>
inline bool Registry::HasComponent(Entity entity)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	return entityComponentSignatures[entityId].test(componentId, false);;
}

template<typename TComponent>
inline TComponent& Registry::GetComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	return componentPool->Get(entityId);
}

template<typename TSystem, typename ...TArgs>
inline void Registry::AddSystem(TArgs && ...args)
{
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template<typename TSystem>
inline void Registry::RemoveSystem(System system)
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template<typename TSystem>
inline void Registry::HasSystem(System system)
{
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template<typename TSystem>
inline TSystem& Registry::GetSystem() const
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}
