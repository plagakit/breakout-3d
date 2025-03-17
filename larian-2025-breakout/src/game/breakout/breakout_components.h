#pragma once

#include "math/math.h"
#include <raylib.h>

// A collection of all the components to be used in the breakout
// scene. Scaling up, these should be put into separate classes,
// but I'm bunching them all here due to the small size of this game.

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
	float radius = 1.0f;
	float height = 1.0f;
	int rings = 8;
	int segments = 16;
};

struct SphereMesh
{
	Color color = BLUE;
	Color outlineColor = DARKBLUE;
	float radius = 1.0f;
	int rings = 8;
	int slices = 16;
};

// Misc.

struct Particle
{
	float timeLeft = 1.0f;
};

// Game logic

struct Gravity
{
	static constexpr float DEFAULT_GRAVITY = 98.0f;
	bool enabled = true;
	float strength = DEFAULT_GRAVITY;
};

struct Player
{
	static constexpr float CAMERA_MOUSE_SENSITIVITY = 0.05f;
	static constexpr Vec3 CAMERA_OFFSET = { 0.0f, 5.0f, -15.0f };
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

	static constexpr float DEFAULT_JUMP_STR = 40.0f;
	bool isGrounded = true;
	float jumpStr = DEFAULT_JUMP_STR;

	static constexpr float DEFAULT_ATTACK_COOLDOWN = 1.0f;
	bool canAttack = true;
	float attackCooldown = DEFAULT_ATTACK_COOLDOWN;
	float liveAttackCooldown = 0.0f;

	static constexpr float DEFAULT_DASH_COOLDOWN = 2.0f;
	static constexpr float DASH_TIME = 0.5f;
	bool canDash = true;
	bool isDashing = false;
	float liveDashTimer = DASH_TIME;
	float dashCooldown = DEFAULT_DASH_COOLDOWN;
	float liveDashCooldown = 0.0f;
	float dashSpeed = DEFAULT_SPEED * 4.0f;
	Vec3 dashDirection = { 0.0f, 0.0f, 0.0f };
};

struct PlayerHitbox
{
	static constexpr float TIME_ALIVE = 0.25f;
	static constexpr float RADIUS = 5.0f;
	static constexpr float OFFSET_MLTPLR = 1.15f;

	Vec3 returnDirection = { 0.0f, 0.0f, 0.0f };
};

struct Wall
{
	static constexpr float THICKNESS = 100.0f;

	bool isGround = false;
	Vec3 normal = { 0.0f, 0.0f, 0.0f };
	Vec2 planeSize = { 0.0f, 0.0f };
};

struct Ball
{
	static constexpr float DEFAULT_RADIUS = 1.0f;
	bool lastHitByPlayer = false;

	// The ball can reach a speed above the hard limit, but then quickly damps down to soft limit
	static constexpr float HARD_MAX_SPEED = 22.0f;
	static constexpr float SOFT_MAX_SPEED = 15.0f;
	static constexpr float DAMP_RATE = 0.1f;
	static constexpr float DEFAULT_SPEED = 13.0f;
	static constexpr float PLAYER_HIT_SPEED_MULTIPLIER = 1.25f;
	static constexpr float DEFAULT_GRAVITY_STRENGTH = 50.0f;

	static constexpr Color NORMAL_PARTICLE_COLOR = Color{ 253, 249, 0, 100 };
	static constexpr Color CURVE_PARTICLE_COLOR = Color{ 230, 41, 55, 100 };
	static constexpr Color GRAVITY_PARTICLE_COLOR = Color{ 0, 121, 241, 100 };
	static constexpr float PARTICLE_SPEED_THRESHOLD = SOFT_MAX_SPEED + 0.5f;
	static constexpr float PARTICLE_SPAWN_PERIOD = 0.1f;
	static constexpr float PARTICLE_LIFETIME = 0.3f;
	float particleSpawnTimer = PARTICLE_SPAWN_PERIOD;
};

struct CurveModifier
{
	static constexpr float DEFAULT_CURVE_STRENGTH = 25.0f;
	float strength = DEFAULT_CURVE_STRENGTH;
	Vec3 direction = { 0.0f, 0.0f, 0.0f };
};

struct Brick
{
	bool wasJustHit = false;
	Entity wasJustHitBall = NULL_ENTITY;

	int health = 1;
	int points = 100;

	enum Type
	{
		NORMAL,
		CURVE,
		GRAVITY,
		ADD_BALLS
	};
	Type type = Type::CURVE;
};