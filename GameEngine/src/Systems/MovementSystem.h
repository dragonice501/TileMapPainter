#pragma once

#include "../ECS/ESC.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"

class MovementSystem: public System
{
public:
	MovementSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<RigibodyComponent>();
	}

	void Update(float deltaTime)
	{
		for (auto entity : GetSystemEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto rigidbody = entity.GetComponent<RigibodyComponent>();

			transform.position += rigidbody.velocity * deltaTime;

			//std::cout << "Entity " << entity.GetId() << " position is now " << transform.position << std::endl;
		}
	}

private:

};