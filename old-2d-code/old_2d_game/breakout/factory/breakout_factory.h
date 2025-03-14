//#pragma once
//
//#include "math/math.h"
//#include "ecs/entity_manager.h"
//#include "resource/rl_texture.h"
//
//// A class that handles the creation of most game objects
//// since all game objects are just collections of components.
//// Like the components and systems, scaling up this should be put into multiple classes.
//class BreakoutFactory
//{
//public:
//	BreakoutFactory(EntityManager& registry);
//
//	Entity CreateBall(const Vec2& position, float radius, Texture2D sprite);
//	Entity CreateCollisionParticle(const Vec2& position, Texture2D sprite);
//
//	Entity CreateBrick(const Vec2& position, uint8_t health, float width, float height);
//	void CreateGrid(const Vec2& topLeft, const Vec2& bottomRight, uint8_t health, int rows, int cols, float spacing);
//
//private:
//	EntityManager& m_registry;
//};