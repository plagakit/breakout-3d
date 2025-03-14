//#include "breakout_scene.h"
//
//#include "math/colliders.h"
//#include "game/breakout/components/breakout_components.h"
//
//#include <raylib.h>
//#include <iostream>
//
//struct Circle 
//{
//	Color color;
//	float radius;
//};
//
//BreakoutScene::BreakoutScene(EntityManager& registry) :
//	m_registry(registry),
//	m_systems(registry),
//	m_factory(registry),
//	m_txBall("res/sprites/ball.png"),
//	m_txBallFire("res/sprites/ball_fire.png")
//{
//	m_registry.ReserveEntities(RESERVED_ENTITIES);
//	
//	m_registry.RegisterComponentType<Transform2D>(RESERVED_ENTITIES);
//	m_registry.RegisterComponentType<Texture2D>(RESERVED_ENTITIES);
//	m_registry.RegisterComponentType<Particle>(RESERVED_ENTITIES);
//	m_registry.RegisterComponentType<Circle>(RESERVED_ENTITIES);
//
//	m_registry.RegisterComponentType<CircleCollider>(RESERVED_ENTITIES);
//	m_registry.RegisterComponentType<AABBCollider>(RESERVED_ENTITIES);
//
//	m_registry.RegisterComponentType<Ball>(RESERVED_ENTITIES);
//	m_registry.RegisterComponentType<Brick>(RESERVED_ENTITIES);
//
//	//for (int i = 0; i < RESERVED_ENTITIES; i++)
//	//{
//	//	m_factory.CreateBall({ 100.0f, 100.0f }, 16.0f, m_txBall.GetTexture());
//	//}
//	//m_factory.CreateGrid({ 72.0f, 48.0f }, { 1080.0f, 600.0f }, 250, 10, 10, 50.0f);
//}
//
//void BreakoutScene::Update(float dt)
//{
//	m_systems.UpdateParticles(dt);
//
//	m_systems.UpdateMovement(dt);
//	m_systems.ProcessBallWallCollisions();
//	m_systems.ProcessBallBrickCollisions();
//	
//	m_systems.UpdateBricks();
//}
//
//void BreakoutScene::Render()
//{
//	for (auto [id, tf, circle] : m_registry.AllWith<Transform2D, Circle>())
//	{
//		DrawCircle(
//			static_cast<int>(tf.position.x), 
//			static_cast<int>(tf.position.y), 
//			circle.radius, circle.color);
//	}
//
//	m_systems.RenderBricks();
//	m_systems.RenderBalls();
//}
