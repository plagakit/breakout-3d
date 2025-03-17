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

	// Initialize game
	m_factory.RegisterRequiredComponents();

	m_camera = { 0 };
	m_camera.up = { 0.0f, 1.0f, 0.0f };
	m_camera.fovy = 45.0f;
	m_camera.projection = CAMERA_PERSPECTIVE;
	m_camera.position = { -10.0f, 10.0f, 0.0f };
	m_camera.target = { 0.0f, 0.0f, 0.0f };

	// Create entities
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
	
	m_factory.CreateBrickGrid(
		{ -ROOM_WIDTH + 0.5f, 0.5f, 20.0f },
		{ ROOM_WIDTH - 0.5f, ROOM_HEIGHT - 0.5f, 30.0f },
		{ 5, 4, 2 },
		{ 0.5f, 0.5f, 1.0f },
		1
	);

	StartGame();
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
			WinGame();
		else if (m_registry.Size<Ball>() == 0)
			LoseGame();
	}
	else if (m_gameState == GameState::START_GAME_TRANSITION)
	{
		if (m_transitionTimer <= 0.0f)
		{
			m_gameState = GameState::PLAYING;

			for (int i = 0; i < 10; i++)
			{
				Entity mainBall = m_factory.CreateBall(BALL_START_POS, BALL_RADIUS);

				// Send ball flying in random dir towards bricks
				float randAng = randf() * PI * 2.0f;
				Transform3D& tf = m_registry.Get<Transform3D>(mainBall);
				tf.velocity = Vector3Normalize({ cosf(randAng), sinf(randAng), 1.0f }) * Ball::DEFAULT_SPEED;
			}

		}
	}
	else if (m_gameState == GameState::END_GAME_TRANSITION)
	{

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
	DrawText(TextFormat("%d", m_points), 0, 0, 32, WHITE);
}

void Breakout::StartGame()
{
	Transform3D& playerTF = m_registry.Get<Transform3D>(m_player);
	playerTF.position = PLAYER_START_POS;
	m_camera.position = PLAYER_START_POS + Player::CAMERA_OFFSET;
	m_camera.target = PLAYER_START_POS + Player::CAMERA_LOOK_OFFSET;

	m_transitionTimer = START_GAME_TRANS_TIME;
	m_gameState = GameState::START_GAME_TRANSITION;
}

void Breakout::LoseGame()
{
	m_gameState = GameState::HALTING;
}

void Breakout::WinGame()
{
	m_gameState = GameState::HALTING;
}
