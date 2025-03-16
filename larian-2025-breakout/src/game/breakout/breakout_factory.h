#pragma once

#include "math/math.h"
#include "ecs/entity_manager.h"
#include <raylib.h>

class BreakoutFactory
{
public:
	BreakoutFactory(EntityManager& registry);
	void RegisterRequiredComponents();

	Entity CreateWall(const Vec3& position, const Vec3& normal, const Vec2& size);

	Entity CreatePlayer(const Vec3& position);
	Entity CreatePlayerHitbox(const Vec3& position, const Vec3& returnDirection);

	Entity CreateBall(const Vec3& position, float radius);
	//Entity CreateBall(const Vec3& position, float radius, const Texture2D& texture);

	Entity CreateBrick(const Vec3& position, const Vec3& size, int health);
	void CreateBrickGrid(const Vec3& bottomLeft, const Vec3& topRight, const Vec3& slices, float spacing, uint8_t health);

private:
	EntityManager& m_registry;

	static constexpr size_t PARTICLE_RESERVE = 500;
	static constexpr size_t WALL_RESERVE = 6;
	static constexpr size_t PLAYER_RESERVE = 1;
	static constexpr size_t HITBOX_RESERVE = 3;
	static constexpr size_t BALL_RESERVE = 50;
	static constexpr size_t BRICK_RESERVE = 250;

};