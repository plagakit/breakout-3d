#pragma once

// The different colliders used in the breakout game
// Scaling up, this would be separated into multiple classes,
// but we can put them all here now for conciseness.

#include "math/math.h"

struct AABBCollider
{
	float width;  // x
	float height; // y
	float length; // z
};

struct SphereCollider
{
	float radius;
};