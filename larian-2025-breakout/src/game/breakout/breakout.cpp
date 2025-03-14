#include "breakout.h"

Breakout::Breakout(EntityManager& registry) :
	m_registry(registry),
	m_factory(registry),
	m_system(registry, m_factory),
	m_txtBall("res/sprites/ball.png")
{
	// Register all required components
	m_registry.ReserveEntities(RESERVE_SIZE);
	m_registry.RegisterComponentType<Transform3D>(RESERVE_SIZE);
	
	m_registry.RegisterComponentType<Brick>(300);
	m_registry.RegisterComponentType<AABBCollider>(RESERVE_SIZE);

	m_registry.RegisterComponentType<Ball>(10);
	m_registry.RegisterComponentType<SphereCollider>(RESERVE_SIZE);
	m_registry.RegisterComponentType<BillboardSprite>(RESERVE_SIZE);

	m_registry.RegisterComponentType<CapsuleMesh>(1);
	m_registry.RegisterComponentType<Player>(1);
	m_registry.RegisterComponentType<PlayerHitbox>(5);
	m_registry.RegisterComponentType<Particle>(RESERVE_SIZE);

	m_player = m_factory.CreatePlayer({ 0.0f, 0.0f, 0.0f });

	// Define the camera to look into our 3d world
	m_camera = { 0 };
	m_camera.up = { 0.0f, 1.0f, 0.0f };
	m_camera.fovy = 45.0f;
	m_camera.projection = CAMERA_PERSPECTIVE;

	for (int i = 0; i < 1; i++)
	{
		Entity e = m_factory.CreateBall({ 0.0f, 3.0f, 0.0f }, 0.5f, m_txtBall.GetTexture());

		float angle = randf() * 2.0f * PI;
		float ang2 = randf() * 2.0f * PI;
		Transform3D& tf = m_registry.Get<Transform3D>(e);
		tf.velocity = Vector3Normalize({ cos(angle), cos(ang2), sin(angle)}) * 10.0f;
		//tf.acceleration = { 0.0f, -0.98f, 0.0f };
	}

	m_factory.CreateBrickGrid(
		{ -10.0f, 0.0f, 15.0f },
		{ 10.0f, 10.0f, 17.0f },
		{ 5, 5, 1 },
		1.0f,
		10
	);

	/*for (int i = 0; i < 10; i++)
	{
		Entity e = m_factory.CreateBrick({ 0.0f, 0.0f, i * 7.5f }, { 5.0f, 5.0f, 5.0f }, 250);
	}*/
}

void Breakout::Update(float dt)
{
	m_system.UpdateParticles(dt);

	//UpdateCamera(&m_camera, CAMERA_THIRD_PERSON);
	m_system.UpdatePlayer(dt, m_player);
	m_system.UpdatePlayerCamera(dt, m_camera, m_player);

	m_system.UpdateMovement(dt);
	m_system.BounceBallsOffWalls(dt);
	m_system.ProcessBallCollisions();

	m_system.UpdateBricks();
}

void Breakout::Render()
{
	m_system.Render3DScene(m_camera);
	m_system.RenderPlayerUI(m_player);
}
