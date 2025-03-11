#include "breakout_scene.h"

#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <execution>

#define randf() (float)rand() / RAND_MAX

struct Ball {
	Vector2 position;
	Vector2 velocity;
};

struct Circle {
	Color color;
	float radius;
};


BreakoutScene::BreakoutScene()
{
	registry.ReserveEntities(RESERVED_ENTITIES);
	registry.RegisterComponentType<Ball>(RESERVED_ENTITIES);
	registry.RegisterComponentType<Circle>(RESERVED_ENTITIES);

	for (int i = 0; i < RESERVED_ENTITIES; i++)
	{
		Entity e = registry.CreateEntity();

		Ball b{};
		b.position = { 100.0f, 100.0f };
		b.velocity = Vector2Scale(Vector2Normalize({ randf() * 2.0f - 1, randf() * 2.0f - 1 }), (randf() + 1) * 5.0f);
		
		Circle c;
		c.radius = (randf() + 3.0f) * .8f;
		c.color = GREEN;

		registry.Add<Ball>(e, b);
		registry.Add<Circle>(e, c);
	}
}

void BreakoutScene::Update(float dt)
{
	auto it = registry.AllWith<Ball>();

	std::for_each(
		std::execution::par,
		it.begin(),
		it.end(),
		[dt](std::tuple<Entity, Ball&> item)
		{
			auto [id, ball] = item;
			ball.position = Vector2Add(ball.position, Vector2Scale(ball.velocity, dt * 30.0f));
				
			if (ball.position.x < 0 || ball.position.x > 800)
				ball.velocity.x *= -1;

			if (ball.position.y < 0 || ball.position.y > 450)
				ball.velocity.y *= -1;
		}
	);
}

void BreakoutScene::Render()
{
	//int i = 0;
	//for (auto [id, ball, circle] : registry.AllWith<Ball, Circle>())
	//{
	//	i++;
	//	DrawCircle(ball.position.x, ball.position.y, circle.radius, circle.color);
	//}
}
