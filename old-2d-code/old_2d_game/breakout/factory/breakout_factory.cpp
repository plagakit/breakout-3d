//#include "breakout_factory.h"
//
//#include "math/colliders.h"
//#include "game/breakout/components/breakout_components.h"
//
//BreakoutFactory::BreakoutFactory(EntityManager& registry) :
//	m_registry(registry)
//{}
//
//Entity BreakoutFactory::CreateBall(const Vec2& position, float radius, Texture2D sprite)
//{
//	Entity e = m_registry.CreateEntity();
//
//	Transform2D tf{};
//	tf.position = position;
//	float ang = randf() * 2 * PI;
//	tf.velocity = Vec2(cosf(ang), sinf(ang)) * (randf() + 1.0f) * 200.0f;
//
//	sprite.width = static_cast<int>(radius * 2.0f);
//	sprite.height = static_cast<int>(radius * 2.0f);
//
//	CircleCollider cc;
//	cc.radius = radius;
//
//	m_registry.Add<Ball>(e, {});
//	m_registry.Add<Transform2D>(e, tf);
//	m_registry.Add<Texture2D>(e, sprite);
//	m_registry.Add<CircleCollider>(e, cc);
//	return e;
//}
//
//Entity BreakoutFactory::CreateCollisionParticle(const Vec2& position, Texture2D sprite)
//{
//	Entity e = m_registry.CreateEntity();
//
//	Transform2D tf;
//	tf.position = position;
//
//	Particle p;
//	p.timeLeft = 1.0f;
//	
//	m_registry.Add<Transform2D>(e, tf);
//	m_registry.Add<Texture2D>(e, sprite);
//	m_registry.Add<Particle>(e, p);
//	return e;
//}
//
//Entity BreakoutFactory::CreateBrick(const Vec2& position, uint8_t health, float width, float height)
//{
//	Entity e = m_registry.CreateEntity();
//
//	Transform2D tf;
//	tf.position = position;
//
//	AABBCollider col;
//	col.width = width;
//	col.height = height;
//
//	Brick b;
//	b.health = health;
//
//	m_registry.Add<Transform2D>(e, tf);
//	m_registry.Add<AABBCollider>(e, col);
//	m_registry.Add<Brick>(e, b);
//	return e;
//}
//
//void BreakoutFactory::CreateGrid(const Vec2& topLeft, const Vec2& bottomRight, uint8_t health, int rows, int cols, float spacing)
//{
//	float totalW = bottomRight.x - topLeft.x;
//	float totalH = bottomRight.y - topLeft.y;
//
//	float totalSpacingX = (cols - 1) * spacing;
//	float totalSpacingY = (rows - 1) * spacing;
//
//	float width = (totalW - totalSpacingX) / cols;
//	float height = (totalH - totalSpacingY) / cols;
//
//	for (int i = 0; i < rows; i++)
//	{
//		for (int j = 0; j < cols; j++)
//		{
//			float x = topLeft.x + (width + spacing) * j;
//			float y = topLeft.y + (height + spacing) * i;
//			CreateBrick({ x, y }, health, width, height);
//		}
//	}
//;}
