#include "ESC.h"

#include <iostream>

uint32_t IComponent::nextId = 0;

void System::AddEntityToSystem(Entity entity)
{
	entities.push_back(entity);
}

void System::RemoveEntityFromSytem(Entity entity)
{
	entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other)
	{
			return entity == other;
	}));
}

void Registry::Update()
{
	for (auto entity : entitiesToBeAdded)
	{
		AddEntityToSystems(entity);
	}

	entitiesToBeAdded.clear();
}

Entity Registry::CreateEntity()
{
	uint32_t entityId;
	entityId = numEntites++;

	Entity entity(entityId);
	entity.registry = this;
	entitiesToBeAdded.insert(entity);

	if (entityId >= entityComponentSignatures.size())
	{
		entityComponentSignatures.resize(entityId + 1);
	}

	//std::cout << "Entity created with id: " << static_cast<int>(entityId) << std::endl;

	return entity;
}

void Registry::AddEntityToSystems(Entity entity)
{
	const auto entityId = entity.GetId();

	const auto& entityComponentSignature = entityComponentSignatures[entityId];

	for (auto& system : systems)
	{
		const auto& systemComponentSignature = system.second->GetComponentSignature();

		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested)
		{
			system.second->AddEntityToSystem(entity);
		}
	}
}
