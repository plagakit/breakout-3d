#pragma once

// The different colliders used in the breakout game
// Scaling up, this would be separated into multiple classes,
// but we can put them all here now for conciseness.

#include "math/math.h"

struct CircleCollider
{
	Vec2 offset	= { 0.0f, 0.0f };
	float radius = 1.0f;
};

struct AABBCollider
{
	Vec2 offset	= { 0.0f, 0.0f };
	float width	= 1.0f;
	float height = 1.0f;
};

//struct RayCollider
//{
//	Vec2 offset = { 0.0f, 0.0f };
//	Vec2 direction = { 0.0f, 0.0f };
//};