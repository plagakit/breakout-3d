#pragma once

#include "colliders.h"

struct CollisionData
{
	
};


// 2D Code

//struct CollisionData
//{
//	Vec2 position1;
//	Vec2 position2;
//};
//
//struct CollisionResult
//{
//	bool hit = false;
//	Vec2 contactNormal = { 0.0f, 0.0f };
//	Vec2 restitution = { 0.0f, 0.0f };
//	Vec2 contactPos = { 0.0f, 0.0f }; // for rays
//};
//
//// All of the physics solving code in a neat little place
//class CollisionSolver
//{
//public:
//	static CollisionResult Collide(const CollisionData& data, const CircleCollider& c1, const CircleCollider& c2);
//	static CollisionResult Collide(const CollisionData& data, const CircleCollider& c, const AABBCollider& b);
//	static CollisionResult Collide(const CollisionData& data, const AABBCollider& b1, const AABBCollider& b2);
//
//	//static CollisionResult Collide(const CollisionData& data, const RayCollider& r, const AABBCollider& b);
//};