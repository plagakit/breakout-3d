//#pragma once
//
//#include "ecs/entity_manager.h"
//#include "game/breakout/systems/breakout_system.h"
//#include "game/breakout/factory/breakout_factory.h"
//
///**
//The game scene that simulates all the breakout stuff with the balls
//and the walls and the bouncing and stuff.
//*/
//class BreakoutScene
//{
//public:
//	BreakoutScene(EntityManager& registry);
//
//	void Update(float dt);
//	void Render();
//
//private:
//	static constexpr size_t RESERVED_ENTITIES = 500;
//	EntityManager& m_registry;
//
//	BreakoutSystem m_systems;
//	BreakoutFactory m_factory;
//
//	RLTexture m_txBall;
//	RLTexture m_txBallFire;
//
//};