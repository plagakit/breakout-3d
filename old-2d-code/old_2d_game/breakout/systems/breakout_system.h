//#pragma once
//
//#include "ecs/entity_manager.h"
//
//// A class containing all of the systems used for breakout.
//// Similarly to the components file, scaling up this would 
//// probably be split into separate system classes but this
//// is ok for now for the scope of this project.
//class BreakoutSystem
//{
//public:
//	BreakoutSystem(EntityManager& registry);
//
//	void UpdateParticles(float dt);
//
//	void UpdateMovement(float dt);
//	void ProcessBallWallCollisions();
//	void ProcessBallBrickCollisions();
//
//	void HandleBallCollision();
//	
//	void UpdateBricks();
//
//	void RenderBalls();
//	void RenderBricks();
//
//private:
//	EntityManager& m_registry;
//};