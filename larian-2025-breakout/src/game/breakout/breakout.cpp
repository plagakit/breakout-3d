#include "breakout.h"

#include <rlgl.h>

Breakout::Breakout(EntityManager& registry) :
	m_registry(registry),
	m_factory(registry),
	m_system(registry, m_factory)
	//m_txtBall("res/sprites/ball.png")
{
	// Load resources
	Image checkerboard = GenImageChecked(128, 128, 4, 4, { 50, 50, 50, 255 }, { 75, 75, 75, 255 });
	m_txChecker = LoadTextureFromImage(checkerboard);
	SetTextureWrap(m_txChecker, TEXTURE_WRAP_REPEAT);
	UnloadImage(checkerboard);

	Image checkerboardRed = GenImageChecked(256, 256, 1, 1, { 230, 40, 40, 100 }, { 0, 0, 0, 0 });
	m_txEndzone = LoadTextureFromImage(checkerboardRed);
	SetTextureWrap(m_txEndzone, TEXTURE_WRAP_REPEAT);
	UnloadImage(checkerboardRed);

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

	m_factory.CreateWall({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 100.0f, 100.0f }); // down
	m_factory.CreateWall({ 0.0f, ROOM_HEIGHT, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 100.0f, 100.0f }); // up
	m_factory.CreateWall({ -ROOM_WIDTH, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 100.0f, 100.0f }); // left
	m_factory.CreateWall({ ROOM_WIDTH, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 100.0f, 100.0f }); // right
	m_factory.CreateWall({ 0.0f, 0.0f, ROOM_LENGTH }, { 0.0f, 0.0f, -1.0f }, { 100.0f, 100.0f }); // front
	m_factory.CreateWall({ 0.0f, 0.0f, -ROOM_LENGTH }, { 0.0f, 0.0f, 1.0f }, { 100.0f, 100.0f }); // back 

	m_endZone = m_factory.CreateWall({ 0.0f, 0.0f, ROOM_LENGTH - END_ZONE }, { 0.0f, 0.0f, 1.0f }, { 100.0f, 100.0f });
	m_registry.Remove<AABBCollider>(m_endZone);
	m_registry.Get<Transform3D>(m_endZone).position.z = -END_ZONE;
	

	for (int i = 0; i < 1; i++)
	{
		Entity e = m_factory.CreateBall({ 0.0f, 3.0f, 0.0f }, 0.5f/*, m_txtBall.GetTexture()*/);

		Transform3D& tf = m_registry.Get<Transform3D>(e);
		tf.velocity = { 0.0f, 0.0f, Ball::DEFAULT_SPEED };
	}

	//m_factory.CreateBrickGrid(
	//	{ , 1.0f,  },
	//	{ 18.0f, ROOM_HEIGHT - 1.0f,  },
	//	{ 5, 4, 1 },
	//	1.0f,
	//	1
	//);
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
	m_system.UpdateParticles(dt);

	//UpdateCamera(&m_camera, CAMERA_THIRD_PERSON);
	m_system.UpdatePlayer(dt, m_player);
	m_system.UpdatePlayerCamera(dt, m_camera, m_player);

	m_system.UpdateMovement(dt);
	m_system.ProcessPlayerWallCollision();
	m_system.ProcessBallCollisions();

	m_system.UpdateBalls(dt);
	m_system.UpdateBricks();
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
}
