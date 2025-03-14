#include "breakout_factory.h"

#include "game/breakout/breakout_components.h"

BreakoutFactory::BreakoutFactory(EntityManager& registry) :
	m_registry(registry)
{}

Entity BreakoutFactory::CreatePlayer(const Vec3& position)
{
	Entity e = m_registry.CreateEntity();

	Transform3D tf;
	tf.position = position;
	m_registry.Add<Transform3D>(e, tf);

	CapsuleMesh mesh;
	mesh.height = 1.0f;
	mesh.rings = 8;
	mesh.segments = 16;
	m_registry.Add<CapsuleMesh>(e, mesh);

	Player pl;
	m_registry.Add<Player>(e, pl);

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

Entity BreakoutFactory::CreateBall(const Vec3& position, float radius, const Texture2D& texture)
{
	Entity e = m_registry.CreateEntity();

	Transform3D tf;
	tf.position = position;
	m_registry.Add<Transform3D>(e, tf);

	SphereCollider col;
	col.radius = radius;
	m_registry.Add<SphereCollider>(e, col);

	BillboardSprite spr;
	spr.texture = texture;
	spr.scale = radius * 2.0f;
	m_registry.Add<BillboardSprite>(e, spr);

	Ball ball;
	m_registry.Add<Ball>(e, ball);

	return e;
}

Entity BreakoutFactory::CreateBrick(const Vec3& position, const Vec3& size, int health)
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

	Brick brick;
	brick.health = health;
	m_registry.Add<Brick>(e, brick);

	return e;
}

void BreakoutFactory::CreateBrickGrid(const Vec3& bottomLeft, const Vec3& topRight, const Vec3& slices, float spacing, uint8_t health)
{
	Vec3 totalSize = topRight - bottomLeft;
	Vec3 totalSpacing = Vec3{ slices.x - 1, slices.y - 1, slices.z - 1 } * spacing;
	Vec3 size = (totalSize - totalSpacing) / slices;

	for (int i = 0; i < slices.x; i++)
	{
		for (int j = 0; j < slices.y; j++)
		{
			for (int k = 0; k < slices.z; k++)
			{
				float x = bottomLeft.x + (size.x + spacing) * i;
				float y = bottomLeft.y + (size.y + spacing) * j;
				float z = bottomLeft.z + (size.z + spacing) * k;
				CreateBrick({ x, y, z }, size, health);
			}
		}
	}
}
