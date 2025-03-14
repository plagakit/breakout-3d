#pragma once

#include "math/math.h"
#include <raylib.h>

// 3D Movement

struct Transform3D
{
	Vec3 position = { 0.0f, 0.0f, 0.0f };
	Vec3 velocity = { 0.0f, 0.0f, 0.0f };
	Vec3 acceleration = { 0.0f, 0.0f, 0.0f };
	Vec3 rotation = { 0.0f, 0.0f, 0.0f };
};

// 3D Collision

#include "math/colliders.h"

// Rendering

struct BillboardSprite
{
	Texture2D texture;
	float scale = 1.0f;
};

struct CapsuleMesh
{
	Color color = { 222, 105, 255, 255 };
	Color outlineColor = { 115, 0, 148, 255 };
	float height = 1.0f;
	int rings = 8;
	int segments = 16;
};

// Misc.

struct Particle
{
	float timeLeft = 1.0f;
};

// Game logic

struct Player
{
	static constexpr float CAMERA_MOUSE_SENSITIVITY = 0.05f;
	static constexpr Vec3 CAMERA_OFFSET = { 0.0f, 3.0f, -7.0f };
	static constexpr Vec3 CAMERA_LOOK_OFFSET = { 0.0f, 2.0f, 0.0f };
	static constexpr float CAMERA_MAX_PITCH = 1.0f;
	static constexpr float CAMERA_MIN_PITCH = -1.0f;

	// These could better be encapsulated by adding rotation to transforms
	// but since players are the only thing that rotate in the game this works
	float yaw = 0.0f;
	float pitch = 0.0f;
	Vec3 cameraForward = { 0.0f, 0.0f, 0.0f };

	static constexpr float DEFAULT_SPEED = 10.0f;
	float speed = DEFAULT_SPEED;

	static constexpr float DEFAULT_ATTACK_COOLDOWN = 1.5f;
	bool isAttacking = false;
	float attackCooldown = DEFAULT_ATTACK_COOLDOWN;
	float liveAttackCooldown = 0.0f;

	static constexpr float PLAYER_GRAVITY = -40.0f;
	static constexpr float DEFAULT_JUMP_STR = 20.0f;
	bool isGrounded = true;
	float jumpStr = DEFAULT_JUMP_STR;

	static constexpr float DEFAULT_DASH_COOLDOWN = 1.0f;
	bool isDashing = false;
	float dashCooldown = DEFAULT_DASH_COOLDOWN;
	float liveDashCooldown = 0.0f;
};

struct PlayerHitbox
{
	static constexpr float TIME_ALIVE = 0.25f;
	static constexpr float RADIUS = 4.0f;
	static constexpr float OFFSET_MLTPLR = 1.5f;

	Vec3 returnDirection = { 0.0f, 0.0f, 0.0f };
};

struct Ball
{

};

struct Brick
{
	bool wasJustHit = false;
	int health = 1;
};