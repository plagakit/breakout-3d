#include "collision_solver.h"

// I'll implement these as I need them
// Tuesday: circle vs aabb for ball against brick

#include <algorithm>

// Circle vs. Circle
//CollisionResult CollisionSolver::Collide(const CollisionData& data, const CircleCollider& c1, const CircleCollider& c2)
//{
//    CollisionResult result{};
//    return result;
//}
//
//// Circle vs. AABB
//CollisionResult CollisionSolver::Collide(const CollisionData& data, const CircleCollider& c, const AABBCollider& b)
//{
//	// Partially lifted from https://github.com/plagakit/ubisoft-next-2025/blob/main/Engine/src/physics/collision/collision_solver.cpp
//	// but modified to be cleaner (the contact normal calculation)
//
//	CollisionResult result{};
//	result.hit = false;
//
//	Vec2 cPos = data.position1 + c.offset;
//	Vec2 bPos = data.position2 + b.offset;
//	Vec2 half = Vec2{ b.width, b.height } * 0.5f;
//	Vec2 bMinPos = bPos - half;
//	Vec2 bMaxPos = bPos + half;
//
//	// Calculate the closest corner on box to circle center
//	Vec2 closest;
//	closest.x = std::min(std::max(cPos.x, bMinPos.x), bMaxPos.x);
//	closest.y = std::min(std::max(cPos.y, bMinPos.y), bMaxPos.y);
//
//	float distSq = glm::length2(cPos - closest);
//	if (distSq <= c.radius * c.radius)
//	{
//		result.hit = true;
//		float dist = sqrtf(distSq);
//		
//		// Calculate contact normal by projecting onto an axis of closest wall
//		Vec2 normalDir = (cPos - closest) / dist;
//		if (std::abs(normalDir.x) > std::abs(normalDir.y)) 
//		{
//			result.contactNormal.x = (normalDir.x > 0) ? 1.0f : -1.0f;
//			result.contactNormal.y = 0.0f;
//		}
//		else 
//		{
//			result.contactNormal.x = 0.0f;
//			result.contactNormal.y = (normalDir.y > 0) ? 1.0f : -1.0f;
//		}
//		
//		// Minimum distance needed to move them out of eachother is:
//		result.restitution = result.contactNormal * (c.radius - dist);
//	}
//
//	return result;
//}
//
//// AABB vs. AABB
//CollisionResult CollisionSolver::Collide(const CollisionData& data, const AABBCollider& b1, const AABBCollider& b2)
//{
//    CollisionResult result{};
//    return result;
//}
//
//// Ray vs. AABB
////CollisionResult CollisionSolver::Collide(const CollisionData& data, const RayCollider& r, const AABBCollider& b)
////{
////	CollisionResult result{};
////
////	return result;
////}
