#include "collision_solver.h"

// I'll implement these as I need them

#include <algorithm>

// Sphere vs. Sphere
CollisionResult CollisionSolver::Solve(const CollisionData& data, const SphereCollider& c1, const SphereCollider& c2)
{
	CollisionResult result{};

	Vec3 diff = data.pos2 - data.pos1;
	float distSq = Vector3LengthSqr(diff);
	float radius = c1.radius + c2.radius;
	if (distSq < radius)
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
