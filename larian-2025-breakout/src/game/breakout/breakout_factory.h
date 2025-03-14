#pragma once

#include "math/math.h"
#include "ecs/entity_manager.h"
#include <raylib.h>

class BreakoutFactory
{
public:
	BreakoutFactory(EntityManager& registry);

	Entity CreatePlayer(const Vec3& position);
	Entity CreatePlayerHitbox(const Vec3& position, const Vec3& returnDirection);

	Entity CreateBall(const Vec3& position, float radius, const Texture2D& texture);
	Entity CreateBrick(const Vec3& position, const Vec3& size, int health);
	void CreateBrickGrid(const Vec3& bottomLeft, const Vec3& topRight, const Vec3& slices, float spacing, uint8_t health);

private:
	EntityManager& m_registry;

};