#include "breakout_scene.h"

#include "math/colliders.h"
#include "game/breakout/components/breakout_components.h"

#include <raylib.h>
#include <iostream>

struct Circle 
{
	Color color;
	float radius;
};

BreakoutScene::BreakoutScene(EntityManager& registry) :
	m_registry(registry),
	m_systems(registry),
	m_factory(registry)
{
	m_registry.ReserveEntities(RESERVED_ENTITIES);
	
	m_registry.RegisterComponentType<Transform2D>(RESERVED_ENTITIES);
	m_registry.RegisterComponentType<Texture2D>(RESERVED_ENTITIES);
	m_registry.RegisterComponentType<Particle>(RESERVED_ENTITIES);
	m_registry.RegisterComponentType<Circle>(RESERVED_ENTITIES);

	m_registry.RegisterComponentType<CircleCollider>(RESERVED_ENTITIES);
	m_registry.RegisterComponentType<AABBCollider>(RESERVED_ENTITIES);

	m_registry.RegisterComponentType<Ball>(RESERVED_ENTITIES);
	m_registry.RegisterComponentType<Brick>(RESERVED_ENTITIES);

	for (int i = 0; i < RESERVED_ENTITIES; i++)
	{
		m_factory.CreateBall({ 100.0f, 100.0f }, 16.0f);
	}

	for (int i = 0; i < 100; i++)
	{
		int j = i % 10;
		m_factory.CreateBrick({ 50.0f + (i / 10) * 120.0f, 50.0f + j * 60.0f }, 255, 75.0f, 25.0f);
	}
	//m_factory.CreateBrick({ 300.0f, 150.0f }, 250, 300.0f, 100.0f);

	m_registry.Size<Brick>();
}

void BreakoutScene::Update(float dt)
{
	m_systems.UpdateParticles(dt);

	m_systems.UpdateMovement(dt);
	m_systems.ProcessBallWallCollisions();
	m_systems.ProcessBallBrickCollisions();
	
	m_systems.UpdateBricks();
}

void BreakoutScene::Render()
{
	for (auto [id, tf, circle] : m_registry.AllWith<Transform2D, Circle>())
	{
		DrawCircle(
			static_cast<int>(tf.position.x), 
			static_cast<int>(tf.position.y), 
			circle.radius, circle.color);
	}

	m_systems.RenderBricks();
	m_systems.RenderBalls();
}
