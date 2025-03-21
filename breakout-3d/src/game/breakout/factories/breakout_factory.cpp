#include "breakout_factory.h"

#include "game/breakout/components/breakout_components.h"

BreakoutFactory::BreakoutFactory(EntityManager& registry) :
	m_registry(registry)
{}

void BreakoutFactory::RegisterRequiredComponents()
{
	static constexpr size_t RESERVE = PARTICLE_RESERVE + BALL_RESERVE + BRICK_RESERVE + HITBOX_RESERVE + PLAYER_RESERVE + WALL_RESERVE;

	m_registry.ReserveEntities(RESERVE);
	m_registry.RegisterComponentType<Transform3D>(RESERVE);
	m_registry.RegisterComponentType<Particle>(PARTICLE_RESERVE);

	m_registry.RegisterComponentType<Brick>(BRICK_RESERVE);
	m_registry.RegisterComponentType<BoxMesh>(BRICK_RESERVE);
	m_registry.RegisterComponentType<AABBCollider>(BRICK_RESERVE + WALL_RESERVE + PLAYER_RESERVE);
	m_registry.RegisterComponentType<Wall>(WALL_RESERVE);

	m_registry.RegisterComponentType<Ball>(BALL_RESERVE);
	m_registry.RegisterComponentType<SphereCollider>(BALL_RESERVE + HITBOX_RESERVE);
	m_registry.RegisterComponentType<SphereMesh>(BALL_RESERVE + HITBOX_RESERVE);
	//m_registry.RegisterComponentType<BillboardSprite>(RESERVE_SIZE);

	m_registry.RegisterComponentType<CapsuleMesh>(PLAYER_RESERVE);
	m_registry.RegisterComponentType<Player>(PLAYER_RESERVE);
	m_registry.RegisterComponentType<PlayerHitbox>(HITBOX_RESERVE);

	m_registry.RegisterComponentType<Gravity>(PLAYER_RESERVE);
	m_registry.RegisterComponentType<CurveModifier>(BALL_RESERVE);
}

Entity BreakoutFactory::CreateWall(const Vec3& position, const Vec3& normal, const Vec2& size, bool isGround)
{
	// Walls are gonna be AABBs instead of planes to save me some time
	// but that means I'm gonna have to do some additional math to properly
	// configure them - ex. position is where the "plane" of the wall should be

	Entity e = m_registry.CreateEntity();

	Transform3D tf;
	tf.position = position - normal * Wall::THICKNESS * 0.5f;
	m_registry.Add<Transform3D>(e, tf);

	AABBCollider col;
	if (std::abs(normal.x) > EPSILON) // left or right
	{
		col.width = Wall::THICKNESS;
		col.height = size.y;
		col.length = size.x;
	}
	else if (std::abs(normal.y) > EPSILON) // up or down
	{
		col.width = size.x;
		col.height = Wall::THICKNESS;
		col.length = size.y;
	}
	else // forward or back
	{
		col.width = size.x;
		col.height = size.y;
		col.length = Wall::THICKNESS;
	}
	m_registry.Add<AABBCollider>(e, col);

	Wall w;
	w.normal = normal;
	w.planeSize = size;
	w.isGround = isGround;
	m_registry.Add<Wall>(e, w);

	return e;
}

Entity BreakoutFactory::CreatePlayer(const Vec3& position)
{
	Entity e = m_registry.CreateEntity();

	Transform3D tf;
	tf.position = position;
	m_registry.Add<Transform3D>(e, tf);

	CapsuleMesh mesh;
	m_registry.Add<CapsuleMesh>(e, mesh);

	AABBCollider col;
	col.length = mesh.radius * 1.5f;
	col.width = col.length;
	col.height = mesh.height + mesh.radius * 2.0f;
	m_registry.Add<AABBCollider>(e, col);
	
	Player pl;
	m_registry.Add<Player>(e, pl);

	Gravity g; g.enabled = true;
	m_registry.Add<Gravity>(e, g);

	return e;
}

Entity BreakoutFactory::CreatePlayerHitbox(const Vec3& position, const Vec3& returnDirection)
{
	Entity e = m_registry.CreateEntity();

	Transform3D tf;
	tf.position = position;
	m_registry.Add<Transform3D>(e, tf);

	SphereCollider col;
	col.radius = PlayerHitbox::RADIUS;
	m_registry.Add<SphereCollider>(e, col);

	PlayerHitbox hb;
	hb.returnDirection = returnDirection;
	m_registry.Add<PlayerHitbox>(e, hb);

	Particle p;
	p.timeLeft = PlayerHitbox::TIME_ALIVE;
	m_registry.Add<Particle>(e, p);

	return e;
}

Entity BreakoutFactory::CreateBall(const Vec3& position, const Vec3& velocity, float radius/*, const Texture2D& texture*/)
{
	Entity e = m_registry.CreateEntity();

	Transform3D tf;
	tf.position = position;
	tf.velocity = velocity;
	m_registry.Add<Transform3D>(e, tf);

	SphereCollider col;
	col.radius = radius;
	m_registry.Add<SphereCollider>(e, col);

	//BillboardSprite spr;
	//spr.texture = texture;
	//spr.scale = radius * 2.0f;
	//m_registry.Add<BillboardSprite>(e, spr);

	SphereMesh mesh;
	mesh.radius = radius;
	mesh.color = RAYWHITE;
	mesh.outlineColor = { 170, 170, 170, 255 };
	mesh.rings = 8;
	mesh.slices = 16;
	m_registry.Add<SphereMesh>(e, mesh);

	Ball ball;
	m_registry.Add<Ball>(e, ball);

	return e;
}

Entity BreakoutFactory::CreateBallParticle(const Vec3& position, const Vec3& velocity, const Vec3& acceleration, const Color& color)
{
	Entity e = m_registry.CreateEntity();
	
	Transform3D tf;
	tf.position = position;
	tf.velocity = velocity;
	tf.acceleration = acceleration;
	m_registry.Add<Transform3D>(e, tf);

	Particle p;
	p.timeLeft = Ball::PARTICLE_LIFETIME;
	m_registry.Add<Particle>(e, p);

	SphereMesh mesh;
	mesh.color = color;
	mesh.outlineColor = color;
	mesh.radius = 0.2f;
	m_registry.Add<SphereMesh>(e, mesh);

	return e;
}

Entity BreakoutFactory::CreateBrick(const Vec3& position, const Vec3& size, int health, Brick::Type type, const BrickTypeInfoArr& typeInfos)
{
	Entity e = m_registry.CreateEntity();

	Transform3D tf;
	tf.position = position;
	m_registry.Add<Transform3D>(e, tf);

	AABBCollider col;
	col.width = size.x;
	col.height = size.y;
	col.length = size.z;
	m_registry.Add<AABBCollider>(e, col);

	BoxMesh mesh;
	mesh.size = size;
	mesh.color = typeInfos[type].color;
	mesh.outlineColor = typeInfos[type].outlineColor;
	m_registry.Add<BoxMesh>(e, mesh);

	Brick brick;
	brick.health = health;
	brick.type = type;
	brick.points = typeInfos[type].points;
	m_registry.Add<Brick>(e, brick);

	return e;
}

void BreakoutFactory::CreateBrickGrid(
	const Vec3& bottomLeft, 
	const Vec3& topRight, 
	const BrickGridGenerationParams& params, 
	const BrickTypeInfoArr& typeInfos)
{
	m_brickGenerator.Reset();

	Vec3 totalSize = topRight - bottomLeft;
	Vec3 totalSpacing = Vec3{ params.slices.x - 1, params.slices.y - 1, params.slices.z - 1 } * params.spacing;
	Vec3 size = (totalSize - totalSpacing) / params.slices;

	for (int i = 0; i < params.slices.x; i++)
	{
		for (int j = 0; j < params.slices.y; j++)
		{
			for (int k = 0; k < params.slices.z; k++)
			{
				float x = bottomLeft.x + (size.x + params.spacing.x) * i;
				float y = bottomLeft.y + (size.y + params.spacing.y) * j;
				float z = bottomLeft.z + (size.z + params.spacing.z) * k;
				Vec3 pos = Vec3{ x, y, z } + size * 0.5f;

				CreateBrick(pos, size, params.health, m_brickGenerator.Next(params), typeInfos);
			}
		}
	}
}

//Brick::Type BreakoutFactory::GenerateBrickType(const BrickGridGenerationParams& params)
//{
//	const float total = params.gravBrickChance
//		+ params.curveBrickChance
//		+ params.addBallBrickChance;
//
//	// Normalize if goes over 1
//	const float norm = total > 1.0f ? 1.0f / total : 1.0f;
//
//	const float pick = randf();
//	float accum = 0.0f;
//
//	accum += params.gravBrickChance * norm;
//	if (pick < accum) return Brick::GRAVITY;
//
//	accum += params.curveBrickChance * norm;
//	if (pick < accum) return Brick::CURVE;
//
//	accum += params.addBallBrickChance * norm;
//	if (pick < accum) return Brick::ADD_BALLS;
//
//	return Brick::NORMAL;
//}
