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
	void ProcessBallCollisions();
	void ProcessPlayerWallCollision();

	// Game logic
	void UpdateBalls(float dt);
	void UpdateBricks();
	//bool IsBallInEndZone();

	// Rendering
	void RenderWalls(const Mesh& mesh, const Material& mat);
	void RenderPrimitiveEntities(const Camera3D& camera);
	void RenderBallShadows(const Mesh& quad, const Material& mat);
	void RenderEndzone(Entity endZone, const Mesh& mesh, const Material& mat);
	void RenderPlayerUI(Entity player);

private:
	EntityManager& m_registry;
	BreakoutFactory& m_factory;
};