#include "breakout_system.h"

#include "game/game.h"
#include "math/collision_solver.h"
#include "game/breakout/components/breakout_components.h"
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

#pragma region Player Movement

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
	pData.pitch = std::clamp(pData.pitch, Player::CAMERA_MIN_PITCH, Player::CAMERA_MAX_PITCH);
	Vec3 forward = Vector3Normalize(camera.target - camera.position);
	Vec3 right = Vector3CrossProduct(forward, up);
	targetLocal = Vector3RotateByAxisAngle(targetLocal, right, pData.pitch);
	camera.position = camera.target - targetLocal;

	Vec3 delta = camera.target - camera.position;

	// And make sure it doesn't clip into any walls
	for (auto [id, wall, tf, box] : m_registry.AllWith<Wall, Transform3D, AABBCollider>())
	{
		CollisionData data = { camera.position, tf.position };
		SphereCollider cameraCollider = { 0.001f };
		CollisionResult result = CollisionSolver::Solve(data, cameraCollider, box);
		if (result.hit)
		{
			camera.position -= result.restitution * 1.1f;

			// Do another test so the camera isn't right up against the wall
			// and makes the wall invisible?
			//SphereCollider widerCollider = { 0.01f };
			//CollisionResult wideResult = CollisionSolver::Solve(data, widerCollider, box);
			//camera.position -= wideResult.restitution;
			
			// Push the target out of the wall?
			//CollisionData data2 = { camera.target, tf.position };
			//SphereCollider targetCollider = { 1.0f };
			//CollisionResult result2 = CollisionSolver::Solve(data2, targetCollider, box);
			//if (result2.hit)
			//	camera.target += result2.restitution;

			// Option 3: keep the same rotation as before but just shift it over
			camera.target = camera.position + delta;

			// I think the best option would be to shoot a raycast out of the target
			// towards the camera position to "zoom in" when we're moving out of a wall,
			// but I don't have the time to code that up
		}
	}

	pData.cameraForward = Vector3Normalize(camera.target - camera.position);
}

void BreakoutSystem::UpdatePlayer(float dt, Entity player)
{
	Player& pData = m_registry.Get<Player>(player);
	Gravity& pGrav = m_registry.Get<Gravity>(player);
	Transform3D& pTransform = m_registry.Get<Transform3D>(player);
	CapsuleMesh& pMesh = m_registry.Get<CapsuleMesh>(player);

	// Jump behaviour
	if (pData.airState == Player::AirState::GROUNDED)
	{
		pGrav.enabled = false;

		if (IsKeyPressed(KEY_SPACE))
		{
			pData.airState = Player::AirState::IN_AIR;
			pGrav.enabled = true;
			pTransform.velocity.y = pData.jumpStr;
		}
	}
	
	// Movement behaviour
	pData.liveDashCooldown -= dt;
	pData.canDash = pData.liveDashCooldown <= 0.0f
		&& pData.moveState == Player::MoveState::WALKING;

	if (pData.moveState == Player::MoveState::DASHING)
	{
		float dashProgress = pData.liveDashTimer / Player::DASH_TIME;
		pData.liveDashTimer -= dt;
		if (pData.liveDashTimer <= 0.0f)
		{
			pData.moveState = Player::MoveState::WALKING;
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
	else // moveState == walking or idle
	{
		// Horizontal movement
		float xMove = static_cast<float>(IsKeyDown('A')) - static_cast<float>(IsKeyDown('D'));
		float zMove = static_cast<float>(IsKeyDown('W')) - static_cast<float>(IsKeyDown('S'));
		Vec3 move = Vector3RotateByAxisAngle({ xMove, 0.0f, zMove }, { 0.0f, 1.0f, 0.0f }, pData.yaw);

		// Perform dash
		if (pData.canDash && IsKeyPressed(KEY_LEFT_SHIFT))
		{
			pData.moveState = Player::MoveState::DASHING;
			pData.liveDashCooldown = pData.dashCooldown;
			pData.liveDashTimer = Player::DASH_TIME;
			pData.dashDirection = move;
			pGrav.enabled = false;
		}
		else if (std::abs(xMove) > EPSILON || std::abs(zMove) > EPSILON)
		{
			pData.moveState = Player::MoveState::WALKING;
			pTransform.position += move * pData.speed * dt;
		}
		else
		{
			pData.moveState = Player::MoveState::IDLE;
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
}

#pragma endregion

#pragma region General Movement & Physics

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

void BreakoutSystem::ProcessCollisions()
{
	// In this system we pick and choose which groups of entities we 
	// want to collide. In a larger engine, I think this would take
	// the form of collision groups/layers

	auto balls = m_registry.AllWith<Ball, Transform3D, SphereCollider>();
	auto bricks = m_registry.AllWith<Brick, Transform3D, AABBCollider>();
	auto playerHitboxes = m_registry.AllWith<PlayerHitbox, Transform3D, SphereCollider>();
	auto walls = m_registry.AllWith<Wall, Transform3D, AABBCollider>();
	auto players = m_registry.AllWith<Player, Transform3D, AABBCollider>();

	// Ball vs. ...
	for (auto [blid, ball, bltf, blcol] : balls)
	{
		// Ball vs. Wall
		for (auto [wid, wall, wtf, wcol] : walls)
		{
			CollisionData data = { bltf.position, wtf.position };
			CollisionResult result = CollisionSolver::Solve(data, blcol, wcol);
			if (result.hit)
				OnBallHitWall(blid, wid, result);
		}

		// Ball vs. Brick
		for (auto [brid, brick, brtf, brcol] : bricks)
		{
			CollisionData data = { bltf.position, brtf.position };
			CollisionResult result = CollisionSolver::Solve(data, blcol, brcol);
			if (result.hit)
				OnBallHitBrick(blid, brid, result);
		}

		// Ball vs. PlayerHitbox
		// debounce - a ball must bounce off smth before being hit by a hitbox again
		if (ball.lastHitByPlayer) 
			continue;

		for (auto [hbid, hitbox, hbtf, hbcol] : playerHitboxes)
		{
			CollisionData data = { bltf.position, hbtf.position };
			CollisionResult result = CollisionSolver::Solve(data, blcol, hbcol);
			if (result.hit)
				OnBallHitHitbox(blid, hbid, result);
		}
	}

	// Player vs. ...
	for (auto [pid, player, ptf, pcol] : players)
	{
		// Player vs. Wall
		for (auto [wid, wall, wtf, wcol] : walls)
		{
			CollisionData data = { ptf.position, wtf.position };
			CollisionResult result = CollisionSolver::Solve(data, pcol, wcol);
			if (result.hit)
				OnPlayerHitWall(pid, wid, result);
		}
	}
}

#pragma endregion

#pragma region Game Logic

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

		// Make it so that player can't vertically stall the ball - they gotta be on their toes ;)
		tf.velocity.z = tf.velocity.z < 0.0f
			? std::min(tf.velocity.z, -Ball::MINIMUM_Z_SPEED)
			: std::max(tf.velocity.z, Ball::MINIMUM_Z_SPEED);

		// Spawn particles
		if (speed >= Ball::PARTICLE_SPEED_THRESHOLD)
		{
			ball.particleSpawnTimer -= dt;
			if (ball.particleSpawnTimer <= 0.0f)
			{
				ball.particleSpawnTimer = Ball::PARTICLE_SPAWN_PERIOD;
				
				Vec3 pos = tf.position + RandomPointInSphere() * 0.4f;
				Vec3 vel = tf.velocity;
				Vec3 acc = Vector3Negate(tf.velocity) * 2.0f;

				Color color = m_registry.Has<Gravity>(id) ? Ball::GRAVITY_PARTICLE_COLOR
					: m_registry.Has<CurveModifier>(id) ? Ball::CURVE_PARTICLE_COLOR
					: Ball::NORMAL_PARTICLE_COLOR;

				m_factory.CreateBallParticle(pos, vel, acc, color);
			}
		}
	}
}

void BreakoutSystem::ApplyBallModifiers(float dt)
{
	for (auto [id, curve, tf] : m_registry.AllWith<CurveModifier, Transform3D>())
	{
		tf.velocity += curve.direction * curve.strength * dt;
	}

	// Gravity is handled in movement
}

int BreakoutSystem::DeleteBricksAndGetPoints()
{
	int total = 0;
	for (auto [id, brick] : m_registry.AllWith<Brick>())
	{
		if (brick.health <= 0)
		{
			total += brick.points;
			m_registry.QueueDelete(id);
		}
	}
	return total;
}

void BreakoutSystem::DeleteBallsInEndZone(float endZone)
{
	for (auto [id, ball, tf] : m_registry.AllWith<Ball, Transform3D>())
	{
		if (tf.position.z < -endZone)
			m_registry.QueueDelete(id);
	}
}

void BreakoutSystem::DeleteOutOfBoundsBalls(const Vec3& pos, const Vec3& size)
{
	for (auto [id, ball, tf] : m_registry.AllWith<Ball, Transform3D>())
	{
		bool isOOB = 
			tf.position.x < pos.x - size.x * 0.5f ||
			tf.position.x > pos.x + size.x * 0.5f ||
			tf.position.y < pos.y - size.y * 0.5f ||
			tf.position.y > pos.y + size.y * 0.5f ||
			tf.position.z < pos.z - size.z * 0.5f ||
			tf.position.z > pos.z + size.z * 0.5f;

		if (isOOB)
			m_registry.QueueDelete(id);
	}
}

#pragma endregion

#pragma region Events

void BreakoutSystem::OnBallHitWall(Entity ball, Entity wall, const CollisionResult& result)
{
	// Apply reflection
	Transform3D& ballTF = m_registry.Get<Transform3D>(ball);
	ballTF.position += result.restitution;
	ballTF.velocity = Reflect(ballTF.velocity, result.contactNormal);
	
	// Set data
	Ball& ballData = m_registry.Get<Ball>(ball);
	ballData.lastHitByPlayer = false;

	// Remove curve if hitting the wall we're curving into
	if (m_registry.Has<CurveModifier>(ball))
	{
		CurveModifier& modifier = m_registry.Get<CurveModifier>(ball);
		float dot = Vector3DotProduct(modifier.direction, result.contactNormal);
		if (dot <= -1.0f + EPSILON)
			m_registry.Remove<CurveModifier>(ball);
	}
}

void BreakoutSystem::OnBallHitBrick(Entity ball, Entity brick, const CollisionResult& result)
{
	// Ball behaviour
	Transform3D& ballTF = m_registry.Get<Transform3D>(ball);
	ballTF.position += result.restitution;
	ballTF.velocity = Reflect(ballTF.velocity, result.contactNormal);

	Ball& ballData = m_registry.Get<Ball>(ball);
	ballData.lastHitByPlayer = false;

	// Brick behaviour
	Brick& brickData = m_registry.Get<Brick>(brick);
	brickData.health--;

	// Apply modifiers
	if (brickData.type == Brick::Type::CURVE)
	{
		if (!m_registry.Has<CurveModifier>(ball))
			m_registry.Add<CurveModifier>(ball, {});

		// Curve towards farthest wall on one axis - since we are centered at 0,0 its opposite of position
		CurveModifier& modifier = m_registry.Get<CurveModifier>(ball);
		if (std::abs(ballTF.position.x) > std::abs(ballTF.position.y))
		{
			modifier.direction = { ballTF.position.x > 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f };
			ballTF.velocity.x = 0.0f;
		}
		else
		{
			modifier.direction = { 0.0f, ballTF.position.y > 0.0f ? -1.0f : 1.0f, 0.0f };
			ballTF.velocity.y = 0.0f;
		}
		modifier.strength = CurveModifier::DEFAULT_CURVE_STRENGTH;
	}
	else if (brickData.type == Brick::Type::GRAVITY)
	{
		if (!m_registry.Has<Gravity>(ball))
			m_registry.Add<Gravity>(ball, {});

		Gravity& modifier = m_registry.Get<Gravity>(ball);
		modifier.enabled = true;
		modifier.strength = Ball::DEFAULT_GRAVITY_STRENGTH;
	}
	else if (brickData.type == Brick::Type::ADD_BALLS)
	{
		Vec3 dirs[Brick::ADD_BALLS_AMT];
		GenerateVectorsInCone(result.contactNormal, DEG2RAD * 30.0f, Brick::ADD_BALLS_AMT, dirs);

		float speed = Vector3Length(ballTF.velocity);
		ballTF.velocity = dirs[0] * speed;
		for (int i = 1; i < Brick::ADD_BALLS_AMT; i++)
		{
			Vec3 vel = dirs[i] * speed;
			m_factory.CreateBall(ballTF.position, vel, Ball::DEFAULT_RADIUS);
		}
	}
}

void BreakoutSystem::OnBallHitHitbox(Entity ball, Entity hitbox, const CollisionResult& result)
{
	// Reflect
	const PlayerHitbox& hitboxData = m_registry.Get<PlayerHitbox>(hitbox);
	Transform3D& ballTF = m_registry.Get<Transform3D>(ball);
	
	float speed = Vector3Length(ballTF.velocity);
	ballTF.velocity = hitboxData.returnDirection * speed * Ball::PLAYER_HIT_SPEED_MULTIPLIER;
	
	// Set data
	m_registry.Get<Ball>(ball).lastHitByPlayer = true;

	// Remove modifiers
	if (m_registry.Has<Gravity>(ball))
		m_registry.Remove<Gravity>(ball);

	if (m_registry.Has<CurveModifier>(ball))
		m_registry.Remove<CurveModifier>(ball);
}

void BreakoutSystem::OnPlayerHitWall(Entity player, Entity wall, const CollisionResult& result)
{
	Transform3D& playerTF = m_registry.Get<Transform3D>(player);
	playerTF.position += result.restitution;

	const Wall& wallData = m_registry.Get<Wall>(wall);
	if (wallData.isGround)
	{
		Player& playerData = m_registry.Get<Player>(player);

		playerData.airState = Player::AirState::GROUNDED;
		playerTF.velocity.y = 0.0f;
		playerTF.acceleration.y = 0.0f;
	}
}

#pragma endregion

#pragma region Rendering

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
	// Draw capsules
	for (auto [id, mesh, tf] : m_registry.AllWith<CapsuleMesh, Transform3D>())
	{
		Vec3 half = { 0.0f, mesh.height * 0.5f - mesh.radius, 0.0f };
		Vec3 start = tf.position - half;
		Vec3 end = tf.position + half;
		DrawCapsule(start, end, mesh.radius, mesh.segments, mesh.rings, mesh.color);
		DrawCapsuleWires(start, end, mesh.radius, mesh.segments, mesh.rings, mesh.outlineColor);
	}

	// Draw boxes
	for (auto [id, box, tf] : m_registry.AllWith<BoxMesh, Transform3D>())
	{
		DrawCubeV(tf.position, box.size, box.color);
		DrawCubeWiresV(tf.position, box.size, box.outlineColor);

		// Draw brick health
//#ifdef _DEBUG
//		if (m_registry.Has<Brick>(id))
//		{
//			EndMode3D();
//			const Brick& brick = m_registry.Get<Brick>(id);
//			Vec2 screenPos = GetWorldToScreen(tf.position, camera);
//			DrawText(TextFormat("%d", brick.health), static_cast<int>(screenPos.x), static_cast<int>(screenPos.y), 32, WHITE);
//			BeginMode3D(camera);
//		}
//#endif
	}

	// Draw spheres
	for (auto [id, mesh, tf] : m_registry.AllWith<SphereMesh, Transform3D>())
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
	Matrix translate = MatrixTranslate(tf.position.x, tf.position.y, tf.position.z);
	DrawMesh(mesh, mat, scale * translate);
}

void BreakoutSystem::RenderPlayerUI(Entity player)
{
	// Draw reticle
	int middleX = Game::SCREEN_WIDTH / 2;
	int middleY = Game::SCREEN_HEIGHT / 2;
	DrawLine(middleX - 5, middleY, middleX + 5, middleY, RAYWHITE);
	DrawLine(middleX, middleY - 5, middleX, middleY + 5, RAYWHITE);

	// Controls
	DrawText("Move: WASD", 10, 520, 24, WHITE);
	DrawText("Jump: SPACE", 10, 550, 24, WHITE);

	// Draw cooldown bars
	const Player& pData = m_registry.Get<Player>(player);
	DrawText("Attack: LMB", 10, 580, 24, WHITE);
	DrawText("Dash: LSHIFT", 10, 610, 24, WHITE);
	
	float atkProg = pData.liveAttackCooldown / pData.attackCooldown;
	int atkBarLen = static_cast<int>(200.0f * atkProg);
	DrawRectangle(160, 580, atkBarLen, 24, ColorLerp({ 0, 255, 0, 255 }, RED, atkProg));

	float dashProg = pData.liveDashCooldown / pData.dashCooldown;
	int dashBarLen = static_cast<int>(200.0f * dashProg);
	DrawRectangle(180, 610, dashBarLen, 24, ColorLerp({ 0, 255, 0, 255 }, RED, dashProg));
}

#pragma endregion

#pragma region Helpers

Vec3 BreakoutSystem::Reflect(const Vec3& direction, const Vec3& normal) const
{
	float dot = Vector3DotProduct(direction, normal);
	return direction - normal * (2.0f * dot);
}

Vec3 BreakoutSystem::RandomPointInSphere() const
{
	return Vector3Normalize({ randf(), randf(), randf() }) * randf();
}

void BreakoutSystem::GenerateVectorsInCone(Vector3 direction, float tiltAngle, int n, Vec3 out[])
{
	/*
	* Tilt the direction vector to create n vectors in a circle around it
	* 
	* Example:
	* n = 2, tiltAngle = A = 30 degrees
	* 
	*       direction
			 XXX^XXX     
		   ^X   |   X^   
		  X \   |   / X  
		 X   \  |  /   X 
		X     \A| /     X
		X      \./      X	

	* I dunno how else to explain this. I coded this myself :p
	*/

	// Generate points tilted around (0, 1, 0) with spherical coordinate formula
	for (int i = 0; i < n; i++)
	{
		float azimuth = (float)i / n * 2.0f * PI;
		float x = sinf(tiltAngle) * cosf(azimuth);
		float y = cosf(tiltAngle);
		float z = sinf(tiltAngle) * sinf(azimuth);
		out[i] = { x, y, z };
	}

	// Align up to target direction
	Quaternion rot;
	Vec3 axis = Vector3CrossProduct({ 0.0f, 1.0f, 0.0f }, direction);
	if (Vector3Length(axis) < EPSILON) // if target direction parallel w/ up
	{
		rot = direction.y < 0
			? QuaternionFromAxisAngle({ 1.0f, 0.0f, 0.0f }, PI)
			: QuaternionIdentity();
	}
	else
	{
		float angle = std::acos(Vector3DotProduct({ 0.0f, 1.0f, 0.0f }, direction));
		rot = QuaternionFromAxisAngle(axis, angle);
	}

	// Now rotate all the vectors
	for (int i = 0; i < n; i++)
		out[i] = Vector3RotateByQuaternion(out[i], rot);
}

#pragma endregion
