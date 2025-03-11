#pragma once

#include "ecs/entity_manager.h"

/**
The game scene that simulates all the breakout stuff with the balls
and the walls and the bouncing and stuff.
*/
class BreakoutScene
{
public:
	BreakoutScene();

	void Update(float dt);
	void Render();

private:
	static constexpr size_t RESERVED_ENTITIES = 300000;

	EntityManager registry;

};