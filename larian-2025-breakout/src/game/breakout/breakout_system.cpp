#include "breakout_system.h"

#include "game/breakout/breakout_components.h"
#include "math/collision_solver.h"
#include "game/game.h"
#include <raylib.h>
#include <rlgl.h>
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

	// v this code was too buggy to include
	
	// And make sure it doesn't clip into any walls
	//for (auto [id, wall, tf, box] : m_registry.AllWith<Wall, Transform3D, AABBCollider>())
	//{
	//	CollisionData data = { camera.position, tf.position };
	//	SphereCollider cameraCollider = { 1.0f };
	//	CollisionResult result = CollisionSolver::Solve(data, cameraCollider, box);
	//	if (result.hit)
	//	{
	//		camera.position -= result.restitution;// *2.0f;

	//		// Do another test so the camera isn't right up against the wall
	//		// and makes the wall invisible?
	//		//SphereCollider widerCollider = { 0.01f };
	//		//CollisionResult wideResult = CollisionSolver::Solve(data, widerCollider, box);
	//		//camera.position -= wideResult.restitution;
	//		
	//		// Push the target out of the wall?
	//		CollisionData data2 = { camera.target, tf.position };
	//		SphereCollider targetCollider = { 1.0f };
	//		CollisionResult result2 = CollisionSolver::Solve(data2, targetCollider, box);
	//		if (result2.hit)
	//			camera.target += result2.restitution;
	//	}
	//}

	pData.cameraForward = Vector3Normalize(camera.target - camera.position);
}

void BreakoutSystem::UpdatePlayer(float dt, Entity player)
{
	Player& pData = m_registry.Get<Player>(player);
	Gravity& pGrav = m_registry.Get<Gravity>(player);
	Transform3D& pTransform = m_registry.Get<Transform3D>(player);
	CapsuleMesh& pMesh = m_registry.Get<CapsuleMesh>(player);

	// Jump behaviour
	if (pData.isGrounded)
	{
		pGrav.enabled = false;

		if (IsKeyPressed(KEY_SPACE))
		{
			pData.isGrounded = false;
			pGrav.enabled = true;
			pTransform.velocity.y = pData.jumpStr;
		}
	}
	
	// Attack behaviour
	if (pData.canAttack)
	{
		// Perform attack
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			pData.canAttack = false;
			pData.liveAttackCooldown = pData.attackCooldown;

			Vec3 offset = pData.cameraForward * PlayerHitbox::OFFSET_MLTPLR;
			m_factory.CreatePlayerHitbox(pTransform.position + offset, pData.cameraForward);
		}
	}
	else
	{
		pData.liveAttackCooldown -= dt;
		if (pData.liveAttackCooldown <= 0.0f)
			pData.canAttack = true;
	}

	// Movement behaviour
	if (pData.isDashing)
	{
		float dashProgress = pData.liveDashTimer / Player::DASH_TIME;
		pData.liveDashTimer -= dt;
		if (pData.liveDashTimer <= 0.0f)
		{
			pData.isDashing = false;
			pData.liveDashTimer = 0.0f;
			pGrav.enabled = true;
			pTransform.velocity = { 0.0f, 0.0f, 0.0f };
		}
		else
		{
			// Interpolate dash speed to walk speed
			Vec3 fromVel = pData.dashDirection * pData.dashSpeed;
			Vec3 toVel = pData.dashDirection * pData.speed;
			float t = dashProgress;// *dashProgress; linear feels better than squared
			pTransform.velocity = fromVel * t + toVel * (1.0f - t);
		}
	}
	else
	{
		// Horizontal movement
		float xMove = static_cast<float>(IsKeyDown('A')) - static_cast<float>(IsKeyDown('D'));
		float zMove = static_cast<float>(IsKeyDown('W')) - static_cast<float>(IsKeyDown('S'));
		Vec3 move = Vector3RotateByAxisAngle({ xMove, 0.0f, zMove }, { 0.0f, 1.0f, 0.0f }, pData.yaw);
		pTransform.position += move * pData.speed * dt;

		pData.liveDashCooldown -= dt;
		if (pData.liveDashCooldown <= 0.0f)
			pData.canDash = true;

		// Perform dash
		if (pData.canDash && IsKeyPressed(KEY_LEFT_SHIFT))
		{
			pData.canDash = false;
			pData.isDashing = true;
			pData.liveDashCooldown = pData.dashCooldown;
			pData.liveDashTimer = Player::DASH_TIME;
			pData.dashDirection = move;
			pGrav.enabled = false;
		}
	}

	std::cout << pTransform.position.x << " " << pTransform.position.y << " " << pTransform.position.z << std::endl;
}

// 3D Movement

void BreakoutSystem::UpdateMovement(float dt)
{
	m_registry.ParallelForAll<Gravity, Transform3D>(
		[dt](auto tuple)
		{
			auto [id, gravity, transform] = tuple;

			if (gravity.enabled)
				transform.velocity += Vec3{ 0.0f, -gravity.strength, 0.0f } * dt;
		}
	);

	m_registry.ParallelForAll<Transform3D>(
		[dt](auto tuple)
		{
			auto [id, tf] = tuple;

			tf.velocity += tf.acceleration * dt;
			tf.position += tf.velocity * dt;
		}
	);
}

void BreakoutSystem::ProcessBallCollisions()
{
	auto balls = m_registry.AllWith<Ball, Transform3D, SphereCollider>();
	auto bricks = m_registry.AllWith<Brick, Transform3D, AABBCollider>();
	auto playerHitboxes = m_registry.AllWith<PlayerHitbox, Transform3D, SphereCollider>();
	auto walls = m_registry.AllWith<Wall, Transform3D, AABBCollider>();

	for (auto [blid, ball, bltf, blcol] : balls)
	{
		// Ball vs. Wall
		for (auto [wid, wall, wtf, wcol] : walls)
		{
			CollisionData data = { bltf.position, wtf.position };
			CollisionResult result = CollisionSolver::Solve(data, blcol, wcol);
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

				ball.wasJustHitByPlayer = false;
			}
		}

		// Ball vs. Brick
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
				ball.wasJustHitByPlayer = false;
			}
		}

		// Ball vs. Player hitbox
		if (ball.wasJustHitByPlayer)
			continue;

		for (auto [hbid, hitbox, hbtf, hbcol] : playerHitboxes)
		{
			CollisionData data = { bltf.position, hbtf.position };
			CollisionResult result = CollisionSolver::Solve(data, blcol, hbcol);
			if (result.hit)
			{
				bltf.velocity = hitbox.returnDirection * Vector3Length(bltf.velocity) * Ball::PLAYER_HIT_SPEED_MULTIPLIER;
				ball.wasJustHitByPlayer = true;
			}
		}
	}
}

void BreakoutSystem::ProcessPlayerWallCollision()
{
	auto players = m_registry.AllWith<Player, Transform3D, AABBCollider>();
	auto walls = m_registry.AllWith<Wall, Transform3D, AABBCollider>();
	for (auto [pid, player, ptf, pcol] : players)
	{
		for (auto [wid, wall, wtf, wcol] : walls)
		{
			CollisionData data = { ptf.position, wtf.position };
			CollisionResult result = CollisionSolver::Solve(data, pcol, wcol);
			if (result.hit)
			{
				ptf.position += result.restitution;

				// If wall is roughly ground
				// TODO: change to make more user defined?
				if (Vector3DotProduct(wall.normal, { 0.0f, 1.0f, 0.0f }) > 0.9f)
				{
					player.isGrounded = true;
					ptf.velocity.y = 0.0f;
					ptf.acceleration.y = 0.0f;
				}
			}
		}
	}
}

void BreakoutSystem::UpdateBalls(float dt)
{
	for (auto [id, ball, tf] : m_registry.AllWith<Ball, Transform3D>())
	{
		// Clamp and damp speeds
		float speed = Vector3Length(tf.velocity);
		if (speed > Ball::HARD_MAX_SPEED)
			tf.velocity = tf.velocity * (Ball::SOFT_MAX_SPEED / Ball::HARD_MAX_SPEED);
		else if (speed > Ball::SOFT_MAX_SPEED)
		{
			float newSpeed = speed - Ball::DAMP_RATE * dt;
			tf.velocity = tf.velocity * (newSpeed / speed);
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
				m_registry.QueueDelete(id);
		}
	}
}

//bool BreakoutSystem::IsBallInEndZone()
//{
//	// 
//}

void BreakoutSystem::RenderWalls(const Mesh& mesh, const Material& mat)
{
	//std::cout << rlGetCullDistanceNear() << std::endl;
	for (auto [id, wall, tf, box] : m_registry.AllWith<Wall, Transform3D, AABBCollider>())
	{
		Matrix scale = MatrixScale(wall.planeSize.x, 1.0f, wall.planeSize.y);

		/*Quaternion orientation = QuaternionFromEuler(0.0f, 0.0f, 0.0f);
		Matrix rotate = QuaternionToMatrix(orientation);*/
		//Matrix rotate = MatrixLookAt({ 0.0f, 0.0f, 0.0f }, wall.normal, { 0.0f, 1.0f, 0.0f });

		// Rotate plane such that up faces the normal

		const Vec3 up = { 0.0f, 1.0f, 0.0f };
		float angle = 0.0f;
		Vec3 axis = up;
		float dot = Vector3DotProduct(up, wall.normal);
		if (dot < -1.0f + EPSILON) // edge case of pointing down
		{
			angle = PI;
			axis = { 1.0f, 0.0f, 0.0f };
		}
		else if (dot < 1.0f - EPSILON) // normal case
		{
			angle = std::acosf(dot);
			axis = Vector3CrossProduct(up, wall.normal);
		}
		Matrix rotate = MatrixRotate(axis, angle);

		Vec3 offset = tf.position + Vec3{ box.width, box.height, box.length } * wall.normal * 0.5f;
		Matrix translate = MatrixTranslate(offset.x, offset.y, offset.z);
		DrawMesh(mesh, mat, scale * rotate * translate);
		//DrawCube(tf.position, box.width, box.height, box.length, { 100, 100, 0, 100 });
	}
}

void BreakoutSystem::RenderPrimitiveEntities(const Camera3D& camera)
{
	// Draw players
	for (auto [id, player, mesh, tf] : m_registry.AllWith<Player, CapsuleMesh, Transform3D>())
	{
		Vec3 half = { 0.0f, mesh.height * 0.5f - mesh.radius, 0.0f };
		Vec3 start = tf.position - half;
		Vec3 end = tf.position + half;
		DrawCapsule(start, end, mesh.radius, mesh.segments, mesh.rings, mesh.color);
		DrawCapsuleWires(start, end, mesh.radius, mesh.segments, mesh.rings, mesh.outlineColor);
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

	// Draw balls
	for (auto [id, ball, mesh, tf] : m_registry.AllWith<Ball, SphereMesh, Transform3D>())
	{
		DrawSphereWires(tf.position, mesh.radius, mesh.rings, mesh.slices, mesh.outlineColor);
		DrawSphere(tf.position, mesh.radius, mesh.color);
	}
	//for (auto [id, ball, sprite, tf] : m_registry.AllWith<Ball, BillboardSprite, Transform3D>())
	//{
	//	DrawBillboard(camera, sprite.texture, tf.position, sprite.scale, WHITE);
	//}

	// Draw hitbox
	for (auto [id, hb, col, tf] : m_registry.AllWith<PlayerHitbox, SphereCollider, Transform3D>())
	{
		DrawSphereWires(tf.position, col.radius, 10, 10, BLUE);
		DrawSphere(tf.position, col.radius, { 0, 0, 255, 100 });
	}

	for (auto [id, sp, tf] : m_registry.AllWith<SphereCollider, Transform3D>())
		DrawSphere(tf.position, sp.radius, { 100, 0, 100, 100 });
}

void BreakoutSystem::RenderBallShadows(const Mesh& quad, const Material& mat)
{
	for (auto [id, ball, tf] : m_registry.AllWith<Ball, Transform3D>())
	{
		Matrix m = MatrixScale(2.0f, 1.0f, 2.0f) * MatrixTranslate(tf.position.x, 0.01f, tf.position.z);
		DrawMesh(quad, mat, m);
	}
}

void BreakoutSystem::RenderEndzone(Entity endZone, const Mesh& mesh, const Material& mat)
{
	const Transform3D& tf = m_registry.Get<Transform3D>(endZone);
	const Wall& wall = m_registry.Get<Wall>(endZone);
	Matrix scale = MatrixScale(wall.planeSize.x, 1.0f, wall.planeSize.y);

	// Rotate plane such that up faces the normal
	float dot = Vector3DotProduct({ 0.0f, 1.0f, 0.0f }, wall.normal);
	float angle = std::acosf(dot);
	Vec3 axis = Vector3CrossProduct({ 0.0f, 1.0f, 0.0f }, wall.normal);
	Matrix rotate = MatrixRotate(axis, angle);

	Matrix translate = MatrixTranslate(tf.position.x, tf.position.y, tf.position.z);
	rlDisableBackfaceCulling();
	DrawMesh(mesh, mat, scale * rotate * translate);
	rlEnableBackfaceCulling();
}

void BreakoutSystem::RenderPlayerUI(Entity player)
{
	// Draw reticle
	int middleX = Game::SCREEN_WIDTH / 2;
	int middleY = Game::SCREEN_HEIGHT / 2;
	DrawLine(middleX - 5, middleY, middleX + 5, middleY, RAYWHITE);
	DrawLine(middleX, middleY - 5, middleX, middleY + 5, RAYWHITE);

	// Draw cooldown bars
	const Player& pData = m_registry.Get<Player>(player);
	DrawText("Attack: LMB", 10, 580, 24, WHITE);
	DrawText("Dash: LSHIFT", 10, 610, 24, WHITE);
	
	float atkProg = pData.liveAttackCooldown / pData.attackCooldown;
	int atkBarLen = static_cast<int>(200.0f * atkProg);
	DrawRectangle(160, 580, atkBarLen, 24, ColorLerp({ 0, 255, 0, 255 }, RED, atkProg));

	float dashProg = pData.liveDashCooldown / pData.dashCooldown;
	int dashBarLen = static_cast<int>(200.0f * dashProg);
	DrawRectangle(160, 610, dashBarLen, 24, ColorLerp({ 0, 255, 0, 255 }, RED, dashProg));
}
