#include "breakout.h"

#include <iostream>
#include <rlgl.h>

Breakout::Breakout(EntityManager& registry) :
	m_registry(registry),
	m_factory(registry),
	m_system(registry, m_factory),
	//m_txtBall("res/sprites/ball.png")
	m_points(0)
{
	// Load resources
	Image checkerboard = GenImageChecked(128, 128, 4, 4, { 50, 50, 50, 255 }, { 75, 75, 75, 255 });
	m_txChecker = LoadTextureFromImage(checkerboard);
	SetTextureWrap(m_txChecker, TEXTURE_WRAP_REPEAT);
	UnloadImage(checkerboard);

	//Image checkerboardRed = GenImageChecked(256, 256, 1, 1, { 230, 40, 40, 100 }, { 0, 0, 0, 0 });
	Image red = GenImageColor(100, 100, RED);
	m_txEndzone = LoadTextureFromImage(red);
	SetTextureWrap(m_txEndzone, TEXTURE_WRAP_REPEAT);
	UnloadImage(red);

	m_meshPlane = GenMeshPlane(1.0f, 1.0f, 1, 1);
	m_matPlane = LoadMaterialDefault();
	m_matEndzone = LoadMaterialDefault();
	SetMaterialTexture(&m_matPlane, MATERIAL_MAP_DIFFUSE, m_txChecker);
	SetMaterialTexture(&m_matEndzone, MATERIAL_MAP_DIFFUSE, m_txEndzone);

	Image ballShadow = GenImageGradientRadial(256, 256, 1.0f, BLACK, { 0, 0, 0, 0 });
	m_txBallShadow = LoadTextureFromImage(ballShadow);
	UnloadImage(ballShadow);

	m_meshBallShadow = GenMeshPlane(0.5f, 0.5f, 1, 1);
	m_matBallShadow = LoadMaterialDefault();
	SetMaterialTexture(&m_matBallShadow, MATERIAL_MAP_DIFFUSE, m_txBallShadow);

	m_factory.RegisterRequiredComponents();
	
	RestartGame();
}

Breakout::~Breakout()
{
	UnloadTexture(m_txChecker);
	UnloadTexture(m_txEndzone);
	UnloadMesh(m_meshPlane);
	UnloadMaterial(m_matPlane);
	UnloadMaterial(m_matEndzone);

	UnloadTexture(m_txBallShadow);
	UnloadMesh(m_meshBallShadow);
	UnloadMaterial(m_matBallShadow);
}

void Breakout::Update(float dt)
{
	if (m_transitionTimer > 0.0f)
		m_transitionTimer -= dt;

	if (m_gameState == GameState::PLAYING)
	{
		m_system.UpdateParticles(dt);

		//UpdateCamera(&m_camera, CAMERA_THIRD_PERSON);
		m_system.UpdatePlayer(dt, m_player);
		m_system.UpdatePlayerCamera(dt, m_camera, m_player);

		m_system.UpdateMovement(dt);
		m_system.ProcessCollisions();

		m_system.UpdateBalls(dt);
		m_system.ApplyBallModifiers(dt);

		m_system.DeleteBallsInEndZone(END_ZONE);
		m_system.DeleteOutOfBoundsBalls({ 0.0f, 0.0f, 0.0f }, Vec3{ ROOM_WIDTH, ROOM_HEIGHT, ROOM_LENGTH } * 2.5f);
		m_points += m_system.DeleteBricksAndGetPoints();
		if (m_registry.Size<Brick>() == 0)
			WinRound();
		else if (m_registry.Size<Ball>() == 0)
			LoseLife();
	}
	else if (m_gameState == GameState::ROUND_START_TRANS)
	{
		// Blink sphere mesh color
		SphereMesh& mesh = m_registry.Get<SphereMesh>(m_firstBall);

		// https://www.desmos.com/calculator/cavfrig7bc
		float t = m_transitionTimer / ROUND_START_TRANS_TIME;
		mesh.color.a = 127.5f * sinf(20.0f * t) + 127.5f;

		if (m_transitionTimer <= 0.0f)
		{
			m_gameState = GameState::PLAYING;
			mesh.color = WHITE;
		}
	}
	else if (m_gameState == GameState::ROUND_END_TRANS)
	{
		// Lerp camera back towards start position
		float t = 1.0f - m_transitionTimer / ROUND_END_TRANS_TIME;
		m_camera.position = Vector3Lerp(m_oldCameraPos, PLAYER_START_POS + Player::CAMERA_OFFSET, t);
		m_camera.target = Vector3Lerp(m_oldTargetPos, PLAYER_START_POS + Player::CAMERA_LOOK_OFFSET, t);

		if (m_transitionTimer <= 0.0f)
			StartRound();
	}
}

void Breakout::Render()
{
	BeginMode3D(m_camera);

	m_system.RenderWalls(m_meshPlane, m_matPlane);
	m_system.RenderEndzone(m_endZone, m_meshPlane, m_matEndzone);
	m_system.RenderPrimitiveEntities(m_camera);
	m_system.RenderBallShadows(m_meshBallShadow, m_matBallShadow);

	EndMode3D();

	m_system.RenderPlayerUI(m_player);
	DrawText(TextFormat("Lives: %d", m_lives), 0, 440, 32, WHITE);
	DrawText(TextFormat("Points: %d", m_points), 0, 480, 32, WHITE);
}

void Breakout::RestartGame()
{
	// Initialize game
	m_camera = { 0 };
	m_camera.up = { 0.0f, 1.0f, 0.0f };
	m_camera.fovy = 45.0f;
	m_camera.projection = CAMERA_PERSPECTIVE;
	m_camera.position = { -10.0f, 10.0f, 0.0f };
	m_camera.target = { 0.0f, 0.0f, 0.0f };

	// Default grid gen params, except for slices
	m_gridGenParams.slices = { 5, 3, 2 };
	m_gridGenParams.health = 1;

	// Create entities
	for (auto id : m_registry.All())
		m_registry.QueueDelete(id);
	m_registry.FlushDeleteQueue();

	m_player = m_factory.CreatePlayer({ 0.0f, 0.5f, 0.0f });

	m_factory.CreateWall({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 100.0f, 100.0f }, true); // down
	m_factory.CreateWall({ 0.0f, ROOM_HEIGHT, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 100.0f, 100.0f }, false); // up
	m_factory.CreateWall({ -ROOM_WIDTH, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 100.0f, 100.0f }, false); // left
	m_factory.CreateWall({ ROOM_WIDTH, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 100.0f, 100.0f }, false); // right
	m_factory.CreateWall({ 0.0f, 0.0f, ROOM_LENGTH }, { 0.0f, 0.0f, -1.0f }, { 100.0f, 100.0f }, false); // front
	m_factory.CreateWall({ 0.0f, 0.0f, -ROOM_LENGTH }, { 0.0f, 0.0f, 1.0f }, { 100.0f, 100.0f }, false); // back 

	m_endZone = m_factory.CreateWall({ 0.0f, 0.1f, ROOM_LENGTH - END_ZONE }, { 0.0f, 0.0f, 1.0f }, { 100.0f, 1.0f }, false);
	m_registry.Remove<AABBCollider>(m_endZone);
	m_registry.Get<Transform3D>(m_endZone).position.z = -END_ZONE;

	Transform3D& playerTF = m_registry.Get<Transform3D>(m_player);
	playerTF.position = PLAYER_START_POS;
	m_camera.position = PLAYER_START_POS + Player::CAMERA_OFFSET;
	m_camera.target = PLAYER_START_POS + Player::CAMERA_LOOK_OFFSET;

	m_factory.CreateBrickGrid(
		{ -ROOM_WIDTH + 0.5f, 0.5f, 20.0f },
		{ ROOM_WIDTH - 0.5f, ROOM_HEIGHT - 0.5f, 30.0f },
		m_gridGenParams,
		BRICK_TYPE_INFO
	);

	m_lives = DEFAULT_LIVES;
	m_points = 0;

	StartRound();
}

void Breakout::StartRound()
{
	m_transitionTimer = ROUND_START_TRANS_TIME;
	m_gameState = GameState::ROUND_START_TRANS;

	m_firstBall = m_factory.CreateBall(BALL_START_POS, { 0.0f, 0.0f, 0.0f }, Ball::DEFAULT_RADIUS);
}

void Breakout::EndRound()
{
	m_gameState = GameState::ROUND_END_TRANS;
	m_transitionTimer = ROUND_END_TRANS_TIME;

	m_oldCameraPos = m_camera.position;
	m_oldTargetPos = m_camera.target;

	for (auto [id, ball] : m_registry.AllWith<Ball>())
		m_registry.QueueDelete(id);

	for (auto [id, player, tf] : m_registry.AllWith<Player, Transform3D>())
	{
		tf.position = PLAYER_START_POS;
		player.yaw = 0.0f;
		player.pitch = 0.0f;
	}
}

void Breakout::WinRound()
{
	m_points += 2000; // TODO: remove magic number

	m_factory.CreateBrickGrid(
		{ -ROOM_WIDTH + 0.5f, 0.5f, 20.0f },
		{ ROOM_WIDTH - 0.5f, ROOM_HEIGHT - 0.5f, 30.0f },
		m_gridGenParams,
		BRICK_TYPE_INFO
	);

	EndRound();
}

void Breakout::LoseLife()
{
	m_lives--;
	if (m_lives == 0)
		RestartGame();
	else
		EndRound();
}
