#pragma once

#include "colliders.h"

// Auxilliary data needed to solve a collision
struct CollisionData
{
	Vec3 pos1 = { 0.0f, 0.0f, 0.0f };
	Vec3 pos2 = { 0.0f, 0.0f, 0.0f };
};

struct CollisionResult
{
	bool hit = false;
	Vec3 contactNormal = { 0.0f, 0.0f, 0.0f };
	Vec3 restitution = { 0.0f, 0.0f, 0.0f };
};

// All of the physics solving code in a neat little place
class CollisionSolver
{
public:
	static CollisionResult Solve(const CollisionData& data, const SphereCollider& c1, const SphereCollider& c2);
	static CollisionResult Solve(const CollisionData& data, const SphereCollider& c, const AABBCollider& b);
	static CollisionResult Solve(const CollisionData& data, const AABBCollider& b1, const AABBCollider& b2);
};