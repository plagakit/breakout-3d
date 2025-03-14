#include "breakout_system.h"

#include "game/breakout/breakout_components.h"
#include "math/collision_solver.h"
#include "game/game.h"
#include <raylib.h>
#include <iostream>

BreakoutSystem::BreakoutSystem(EntityManager& registry, BreakoutFactory& factory) :
	m_registry(registry), m_factory(factory)
{}

void BreakoutSystem::UpdateParticles(float dt)
{
	for (auto [id, particle] : m_registry.AllWith<Particle>())
	{
		particle.timeLeft -= dt;
		if (particle.timeLeft <= 0.0f)
			m_registry.QueueDelete(id);
	}
}

void BreakoutSystem::UpdatePlayerCamera(float dt, Camera3D& camera, Entity player)
{
	// Set camera to follow player
	Player& pData = m_registry.Get<Player>(player);
	const Transform3D& pTransform = m_registry.Get<Transform3D>(player);
	camera.position = pTransform.position + Player::CAMERA_OFFSET;
	camera.target = pTransform.position + Player::CAMERA_LOOK_OFFSET;

	// Calc change in rotation
	Vec2 mouseDelta = GetMouseDelta();
	pData.yaw -= mouseDelta.x * dt * Player::CAMERA_MOUSE_SENSITIVITY;
	pData.pitch -= mouseDelta.y * dt * Player::CAMERA_MOUSE_SENSITIVITY;

	// Swivel camera pos to match new yaw
	Vec3 up = Vector3Normalize(camera.up);
	Vec3 targetLocal = camera.target - camera.position;
	targetLocal = Vector3RotateByAxisAngle(targetLocal, up, pData.yaw);
	camera.position = camera.target - targetLocal;

	// And do similar for target pos to match pitch
	//std::cout << pData.cameraPitch << std::endl;
	pData.pitch = std::clamp(pData.pitch, Player::CAMERA_MIN_PITCH, Player::CAMERA_MAX_PITCH);
	Vec3 forward = Vector3Normalize(camera.target - camera.position);
	Vec3 right = Vector3CrossProduct(forward, up);
	targetLocal = Vector3RotateByAxisAngle(targetLocal, right, pData.pitch);
	camera.position = camera.target - targetLocal;

	pData.cameraForward = Vector3Normalize(camera.target - camera.position);
}

void BreakoutSystem::UpdatePlayer(float dt, Entity player)
{
	Player& pData = m_registry.Get<Player>(player);
	Transform3D& pTransform = m_registry.Get<Transform3D>(player);

	// In-air behaviour 
	if (!pData.isGrounded)
	{
		// Reground player
		if (pTransform.position.y < 0.0f)
		{
			pData.isGrounded = true;
			pTransform.position.y = 0.0f;
			pTransform.velocity.y = 0.0f;
			pTransform.acceleration.y = 0.0f;
		}
		else
		{
			pTransform.acceleration.y = Player::PLAYER_GRAVITY;
		}
	}
	// Query jump
	else if (IsKeyPressed(KEY_SPACE))
	{
		pData.isGrounded = false;
		pTransform.velocity.y = Player::DEFAULT_JUMP_STR;
		pTransform.acceleration.y = Player::PLAYER_GRAVITY;
	}
	
	// If not attacking
	if (!pData.isAttacking)
	{
		// Perform attack
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			pData.isAttacking = true;
			pData.liveAttackCooldown = pData.attackCooldown;

			/*Vec3 offset = PlayerHitbox::OFFSET;
			offset = Vector3RotateByAxisAngle(offset, { 0.0f, 1.0f, 0.0f }, pData.yaw);
			offset = Vector3RotateByAxisAngle(offset, { -1.0f, 0.0f, 0.0f }, pData.pitch);*/
			Vec3 offset = pData.cameraForward * PlayerHitbox::OFFSET_MLTPLR;
			m_factory.CreatePlayerHitbox(pTransform.position + offset, pData.cameraForward);
		}
	}
	else
	{
		pData.liveAttackCooldown -= dt;
		if (pData.liveAttackCooldown <= 0.0f)
			pData.isAttacking = false;
	}

	// Horizontal movement
	float xMove = static_cast<float>(IsKeyDown('A')) - static_cast<float>(IsKeyDown('D'));
	float zMove = static_cast<float>(IsKeyDown('W')) - static_cast<float>(IsKeyDown('S'));
	Vec3 move = Vector3RotateByAxisAngle({ xMove, 0.0f, zMove }, { 0.0f, 1.0f, 0.0f }, pData.yaw);
	pTransform.position += move * pData.speed * dt;

	// Dash

}

// 3D Movement

void BreakoutSystem::UpdateMovement(float dt)
{
	m_registry.ParallelForAll<Transform3D>(
		[dt](auto tuple)
		{
			auto [id, tf] = tuple;

			tf.velocity += tf.acceleration * dt;
			tf.position += tf.velocity * dt;
		}
	);
}

void BreakoutSystem::BounceBallsOffWalls(float dt)
{
	m_registry.ParallelForAll<Ball, Transform3D, SphereCollider>(
		[dt](auto tuple)
		{
			auto [id, ball, tf, col] = tuple;

			if (tf.position.y - col.radius < 0)
			{
				tf.position.y = col.radius;
				tf.velocity.y *= -1;
			}
			else if (tf.position.y + col.radius > 10.0f)
			{
				tf.position.y = 10.0f - col.radius;
				tf.velocity.y *= -1;
			}

			if (tf.position.x + col.radius > 5.0f)
			{
				tf.position.x = 5.0f - col.radius;
				tf.velocity.x *= -1;
			}
			else if (tf.position.x - col.radius < -5.0f)
			{
				tf.position.x = -5.0f + col.radius;
				tf.velocity.x *= -1;
			}

			if (tf.position.z + col.radius > 25.0f)
			{
				tf.position.z = 25.0f - col.radius;
				tf.velocity.z *= -1;
			}
			else if (tf.position.z - col.radius < -5.0f)
			{
				tf.position.z = -5.0f + col.radius;
				tf.velocity.z *= -1;
			}
		}
	);
}

void BreakoutSystem::ProcessBallCollisions()
{

	auto balls = m_registry.AllWith<Ball, Transform3D, SphereCollider>();
	auto bricks = m_registry.AllWith<Brick, Transform3D, AABBCollider>();
	auto playerHitboxes = m_registry.AllWith<PlayerHitbox, Transform3D, SphereCollider>();

	for (auto [blid, ball, bltf, blcol] : balls)
	{
		for (auto [brid, brick, brtf, brcol] : bricks)
		{
			CollisionData data = { bltf.position, brtf.position };
			CollisionResult result = CollisionSolver::Solve(data, blcol, brcol);
			if (result.hit)
			{
				// Adjust ball position & velocity
				bltf.position += result.restitution;
				if (std::abs(result.contactNormal.x) > EPSILON)
					bltf.velocity.x *= -1.0f;
				if (std::abs(result.contactNormal.y) > EPSILON)
					bltf.velocity.y *= -1.0f;
				if (std::abs(result.contactNormal.z) > EPSILON)
					bltf.velocity.z *= -1.0f;

				// Set data
				brick.wasJustHit = true;
			}
		}

		for (auto [hbid, hitbox, hbtf, hbcol] : playerHitboxes)
		{
			CollisionData data = { bltf.position, hbtf.position };
			CollisionResult result = CollisionSolver::Solve(data, blcol, hbcol);
			if (result.hit)
			{
				bltf.velocity = hitbox.returnDirection * 10.0f;
			}
		}
	}
}

void BreakoutSystem::UpdateBricks()
{
	for (auto [id, brick] : m_registry.AllWith<Brick>())
	{
		if (brick.wasJustHit)
		{
			brick.wasJustHit = false;
			brick.health--;
			if (brick.health <= 0)
			{
				m_registry.QueueDelete(id);
			}
		}
	}
}

void BreakoutSystem::Render3DScene(const Camera3D& camera)
{
	BeginMode3D(camera);

	DrawGrid(20, 1.0f);

	// Draw capsules
	for (auto [id, mesh, tf] : m_registry.AllWith<CapsuleMesh, Transform3D>())
	{
		Vec3 half = { 0.0f, mesh.height * 0.5f, 0.0f };
		Vec3 start = tf.position - half;
		Vec3 end = tf.position + half;
		DrawCapsule(start, end, 1.0f, mesh.segments, mesh.rings, mesh.color);
		DrawCapsuleWires(start, end, 1.0f, mesh.segments, mesh.rings, mesh.outlineColor);
	}

	// Draw bricks
	for (auto [id, brick, tf, aabb] : m_registry.AllWith<Brick, Transform3D, AABBCollider>())
	{
		DrawCube(tf.position, aabb.width, aabb.height, aabb.length, BLUE);
		DrawCubeWires(tf.position, aabb.width, aabb.height, aabb.length, DARKBLUE);

#ifdef _DEBUG
		EndMode3D();
		Vec2 screenPos = GetWorldToScreen(tf.position, camera);
		DrawText(TextFormat("%d", brick.health), static_cast<int>(screenPos.x), static_cast<int>(screenPos.y), 32, WHITE);
		BeginMode3D(camera);
#endif
	}

	// Draw sprites
	for (auto [id, sprite, tf] : m_registry.AllWith<BillboardSprite, Transform3D>())
	{
		DrawBillboard(camera, sprite.texture, tf.position, sprite.scale, WHITE);
	}

	// Draw colliders
	for (auto [id, col, tf] : m_registry.AllWith<SphereCollider, Transform3D>())
	{
		DrawSphereWires(tf.position, col.radius, 10, 10, WHITE);
		DrawSphere(tf.position, col.radius, { 0, 0, 255, 100 });
	}

	EndMode3D();
}

void BreakoutSystem::RenderPlayerUI(Entity player)
{
	const Player& pData = m_registry.Get<Player>(player);

	// Draw cooldown bars
	DrawText("Attack: LMB", 10, 580, 24, WHITE);
	DrawText("Dash: LSHIFT", 10, 610, 24, WHITE);
	
	float atkProg = pData.liveAttackCooldown / pData.attackCooldown;
	int atkBarLen = static_cast<int>(200.0f * atkProg);
	DrawRectangle(160, 580, atkBarLen, 24, ColorLerp({ 0, 255, 0, 255 }, RED, atkProg));

	float dashProg = pData.liveDashCooldown / pData.dashCooldown;
	int dashBarLen = static_cast<int>(200.0f * dashProg);
	DrawRectangle(160, 580, dashBarLen, 24, ColorLerp({ 0, 255, 0, 255 }, RED, dashProg));
}
