#pragma once

// A collection of all the components to be used in the breakout
// scene. Scaling up, these should be put into separate classes,
// but I'm bunching them all here due to the small size of this game.

#include "math/math.h"

struct Transform2D
{
	Vec2 position = { 0.0f, 0.0f };
	Vec2 velocity = { 0.0f, 0.0f };
	float rotation = 0.0f;
};

struct Particle
{
	float timeLeft = 1.0f;
};

struct Ball
{
	bool collided = false;
};

struct Brick
{
	bool wasJustHit = false;
	uint8_t health = 1;
};

struct Player
{

};