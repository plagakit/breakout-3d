#include "breakout_system.h"

#include "math/math.h"
#include "math/collision_solver.h"
#include "game/breakout/components/breakout_components.h"
#include "game/game.h"

#include <raylib.h>

BreakoutSystem::BreakoutSystem(EntityManager& registry) :
	m_registry(registry)
{}

void BreakoutSystem::UpdateParticles(float dt)
{
	for (auto [id, particle] : m_registry.AllWith<Particle>())
	{
		particle.timeLeft -= dt;
		if (particle.timeLeft < 0)
			m_registry.QueueDelete(id);
	}
}

void BreakoutSystem::UpdateMovement(float dt)
{
	m_registry.ParallelForAll<Transform2D>(
		[dt](auto item)
		{
			auto [id, tf] = item;
			tf.position += tf.velocity * dt;
		}
	);
}

void BreakoutSystem::ProcessBallWallCollisions()
{
	m_registry.ParallelForAll<Ball, Transform2D>(
		[](auto item)
		{
			auto [id, ball, tf] = item;
			if (tf.position.x < 0 || tf.position.x > Game::SCREEN_WIDTH)
				tf.velocity.x *= -1;

			if (tf.position.y < 0 || tf.position.y > Game::SCREEN_HEIGHT)
				tf.velocity.y *= -1;
		}
	);
}

void BreakoutSystem::ProcessBallBrickCollisions()
{
	auto balls = m_registry.AllWith<Ball, Transform2D, CircleCollider>();
	auto bricks = m_registry.AllWith<Brick, Transform2D, AABBCollider>();

	for (auto [ballID, ball, ballTf, ballCol] : balls)
	{
		for (auto [brkID, brk, brkTf, brkCol] : bricks)
		{
			/*float speedSq = glm::length2(ballTf.velocity);
			if (speedSq > some heuristic amount)
			{
				// TODO: "Continuous" collision detection
				// When fast enough, balls clip through the bricks, this could be a solution:
				// We cast a ray from ball's previous position to ball's current position,
				// and get the closest point on that line to the box. If distance is less than,
				// radius, then circle collided at some point - we change its position to that
				// one point and rerun the collision to prevent tunnelling.

				// This problem occurred when I was testing the ball collisions, but for
				// the sake of time I'll fix it if I encounter it again in the design of my
				// game - if the balls get that fast, then I'll code this up.
			}*/

			CollisionData data = { ballTf.position, brkTf.position };
			CollisionResult result = CollisionSolver::Collide(data, ballCol, brkCol);
			if (result.hit)
			{
				// Move outside of brick
				ballTf.position += result.restitution;

				// Reflect off brick
				if (std::abs(result.contactNormal.x) > EPSILON)
					ballTf.velocity.x *= -1.0f;
				if (std::abs(result.contactNormal.y) > EPSILON)
					ballTf.velocity.y *= -1.0f;

				// Set collision
				ball.collided = true;
				brk.wasJustHit = true;
			}
		}
	}
}

void BreakoutSystem::HandleBallCollision()
{
	for (auto [id, ball] : m_registry.AllWith<Ball>())
	{
		if (!ball.collided)
			continue;

		

		ball.collided = false;
	}
}

void BreakoutSystem::UpdateBricks()
{
	for (auto [id, brick] : m_registry.AllWith<Brick>())
	{
		if (brick.wasJustHit)
		{
			brick.health -= 1;
			brick.wasJustHit = false;
		}
			
		if (brick.health <= 0)
			m_registry.QueueDelete(id);
	}
}

void BreakoutSystem::RenderBalls()
{
	for (auto [id, ball, tf, tx] : m_registry.AllWith<Ball, Transform2D, Texture2D>())
	{
		DrawTexture(
			tx,
			static_cast<int>(tf.position.x - tx.width * 0.5f),
			static_cast<int>(tf.position.y - tx.height * 0.5f),
			WHITE
		);
	}
}

void BreakoutSystem::RenderBricks()
{
	for (auto [id, brick, tf, box] : m_registry.AllWith<Brick, Transform2D, AABBCollider>())
	{
		DrawRectangle(
			static_cast<int>(tf.position.x - box.width * 0.5f), 
			static_cast<int>(tf.position.y - box.height * 0.5f),
			static_cast<int>(box.width), 
			static_cast<int>(box.height), 
			BLUE
		);

		DrawText(
			std::to_string(brick.health).c_str(),
			static_cast<int>(tf.position.x),
			static_cast<int>(tf.position.y),
			32, WHITE
		);
	}
}
