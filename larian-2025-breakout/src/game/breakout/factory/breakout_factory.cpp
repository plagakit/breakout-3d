#include "breakout_factory.h"

#include "math/colliders.h"
#include "game/breakout/components/breakout_components.h"

BreakoutFactory::BreakoutFactory(EntityManager& registry) :
	m_registry(registry),
	m_txtBall("res/sprites/ball.png")
{}

Entity BreakoutFactory::CreateBall(const Vec2& position, float radius)
{
	Entity e = m_registry.CreateEntity();

	Transform2D tf{};
	tf.position = position;
	float ang = randf() * 2 * PI;
	tf.velocity = Vec2(cosf(ang), sinf(ang)) * (randf() + 1.0f) * 200.0f;

	Texture2D tx = m_txtBall.GetTexture();
	tx.width = static_cast<int>(radius * 2.0f);
	tx.height = static_cast<int>(radius * 2.0f);

	CircleCollider cc;
	cc.radius = radius;

	m_registry.Add<Ball>(e, {});
	m_registry.Add<Transform2D>(e, tf);
	m_registry.Add<Texture2D>(e, tx);
	m_registry.Add<CircleCollider>(e, cc);
	return e;
}

Entity BreakoutFactory::CreateCollisionParticle(const Vec2& position)
{
	Entity e = m_registry.CreateEntity();

	Transform2D tf;
	tf.position = position;

	Particle p;
	p.timeLeft = 1.0f;
	

	m_registry.Add<Transform2D>(e, tf);
	m_registry.Add<Texture2D>(e, m_txtBall.GetTexture());
	m_registry.Add<Particle>(e, p);
	return e;
}

Entity BreakoutFactory::CreateBrick(const Vec2& position, uint8_t health, float width, float height)
{
	Entity e = m_registry.CreateEntity();

	Transform2D tf;
	tf.position = position;

	AABBCollider col;
	col.width = width;
	col.height = height;

	Brick b;
	b.health = health;

	m_registry.Add<Transform2D>(e, tf);
	m_registry.Add<AABBCollider>(e, col);
	m_registry.Add<Brick>(e, b);
	return e;
}
