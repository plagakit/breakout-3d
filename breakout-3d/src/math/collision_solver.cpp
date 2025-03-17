#include "collision_solver.h"

// I'll implement these as I need them

#include <algorithm>
#include <iostream>

// Sphere vs. Sphere
CollisionResult CollisionSolver::Solve(const CollisionData& data, const SphereCollider& c1, const SphereCollider& c2)
{
	CollisionResult result{};

	Vec3 diff = data.pos2 - data.pos1;
	float distSq = Vector3LengthSqr(diff);
	float radius = c1.radius + c2.radius;
	if (distSq < radius * radius)
	{
		result.hit = true;

		float dist = sqrtf(distSq);
		result.contactNormal = diff / dist;
		result.restitution = result.contactNormal * (radius - dist);
	}

	return result;
}

// Sphere vs. AABB
CollisionResult CollisionSolver::Solve(const CollisionData& data, const SphereCollider& c, const AABBCollider& b)
{
	CollisionResult result{};

	Vec3 half = { b.width * 0.5f, b.height * 0.5f, b.length * 0.5f };
	Vec3 boxMin = data.pos2 - half;
	Vec3 boxMax = data.pos2 + half;

	// Find the closest point on AABB to sphere's center
	Vec3 closestPoint;
	closestPoint.x = std::clamp(data.pos1.x, boxMin.x, boxMax.x);
	closestPoint.y = std::clamp(data.pos1.y, boxMin.y, boxMax.y);
	closestPoint.z = std::clamp(data.pos1.z, boxMin.z, boxMax.z);

	Vec3 diff = data.pos1 - closestPoint;
	float distSq = Vector3LengthSqr(diff);
	if (distSq < c.radius * c.radius)
	{
		result.hit = true;

		// If sphere center inside the box
		if (distSq < EPSILON)
		{
			Vec3 toCenter = data.pos1 - data.pos2;
			float xDepth = half.x - std::abs(toCenter.x);
			float yDepth = half.y - std::abs(toCenter.y);
			float zDepth = half.z - std::abs(toCenter.z);
			float penetration = 0.0f;

			// Find shortest direction to push out
			// Normal is on x-axis
			if (xDepth < yDepth && xDepth < zDepth)
			{
				if (toCenter.x > 0) result.contactNormal = { 1.0f, 0.0f, 0.0f };
				else				result.contactNormal = { -1.0f, 0.0f, 0.0f };
				penetration = c.radius - xDepth;
			}
			// Normal is on y-axis
			else if (yDepth < zDepth)
			{
				if (toCenter.y > 0) result.contactNormal = { 0.0f, 1.0f, 0.0f };
				else				result.contactNormal = { 0.0f, -1.0f, 0.0f };
				penetration = c.radius - yDepth;
			}
			// Normal is on z-axis
			else
			{
				if (toCenter.z > 0) result.contactNormal = { 0.0f, 0.0f, 1.0f };
				else				result.contactNormal = { 0.0f, 0.0f, -1.0f };
				penetration = c.radius - zDepth;
			}

			result.restitution = result.contactNormal * penetration;
		}
		else
		{
			float dist = sqrtf(distSq);
			result.contactNormal = diff / dist;
			result.restitution = result.contactNormal * (c.radius - dist);
		}
	}

	return result;
}

CollisionResult CollisionSolver::Solve(const CollisionData& data, const AABBCollider& b1, const AABBCollider& b2)
{
	CollisionResult result{};

	Vec3 half1 = { b1.width * 0.5f, b1.height * 0.5f, b1.length * 0.5f };
    Vec3 min1 = data.pos1 - half1;
    Vec3 max1 = data.pos1 + half1;

	Vec3 half2 = { b2.width * 0.5f, b2.height * 0.5f, b2.length * 0.5f };
    Vec3 min2 = data.pos2 - half2;
    Vec3 max2 = data.pos2 + half2;

    // Calculate overlaps on each axis
    float overlapX = std::max(0.0f, std::min(max1.x, max2.x) - std::max(min1.x, min2.x));
    float overlapY = std::max(0.0f, std::min(max1.y, max2.y) - std::max(min1.y, min2.y));
    float overlapZ = std::max(0.0f, std::min(max1.z, max2.z) - std::max(min1.z, min2.z));

    // If at least one side is overlapping other
    if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f)
    {
        result.hit = false;
        return result;
    }

	result.hit = true;

    // Determine the axis with minimum overlap to push out of
    float minOverlap = overlapZ;
	if (overlapX < minOverlap) // x
	{
		minOverlap = overlapX;
		float diff = data.pos1.x - data.pos2.x;
		result.contactNormal = (diff > 0.0f) ? Vec3{ 1.0f, 0.0f, 0.0f } : Vec3{ -1.0f, 0.0f, 0.0f };
	}
    else if (overlapY < minOverlap) // y
    {
        minOverlap = overlapY;
		float diff = data.pos1.y - data.pos2.y;
		result.contactNormal = (diff > 0.0f) ? Vec3{ 0.0f, 1.0f, 0.0f } : Vec3{ 0.0f, -1.0f, 0.0f };
    }
	else // z
    {
		float diff = data.pos1.z - data.pos2.z;
		result.contactNormal = (diff > 0.0f) ? Vec3{ 0.0f, 0.0f, 1.0f } : Vec3{ 0.0f, 0.0f, -1.0f };
    }

    result.restitution = result.contactNormal * minOverlap;

	return result;
}
