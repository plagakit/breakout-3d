#pragma once

#include "ecs/entity_manager.h"
#include "game/breakout/breakout_factory.h"
#include <raylib.h>

class BreakoutSystem
{
public:
	BreakoutSystem(EntityManager& registry, BreakoutFactory& factory);

	void UpdateParticles(float dt);

	// Player movement
	void UpdatePlayerCamera(float dt, Camera3D& camera, Entity player);
	void UpdatePlayer(float dt, Entity player);

	// General movement & physics
	void UpdateMovement(float dt);
	void BounceBallsOffWalls(float dt);
	void ProcessBallCollisions();

	// Game logic
	void UpdateBricks();

	// Rendering
	void Render3DScene(const Camera3D& camera);
	void RenderPlayerUI(Entity player);

private:
	EntityManager& m_registry;
	BreakoutFactory& m_factory;
};