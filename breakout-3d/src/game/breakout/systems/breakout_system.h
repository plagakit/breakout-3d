#pragma once

#include "ecs/entity_manager.h"
#include "game/breakout/factories/breakout_factory.h"
#include "math/collision_solver.h"
#include <raylib.h>

// A class containing all of the systems used for breakout.
// Similarly to the components file, scaling up this would 
// probably be split into separate system classes but this
// is ok for now for the scope of this project.
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
	void ProcessCollisions();

	// Game logic
	void UpdateBalls(float dt);
	void ApplyBallModifiers(float dt);
	int DeleteBricksAndGetPoints();
	void DeleteBallsInEndZone(float endZone);
	void DeleteOutOfBoundsBalls(const Vec3& boundsPos, const Vec3& boundsSize);

	// Game events
	// Scaling up, these should use an event design pattern such as observers or an event queue
	// held by each system. For the small scope of breakout, I think this will do for now so I don't
	// have to write a lot of code to set up the event system
	void OnBallHitWall(Entity ball, Entity wall, const CollisionResult& result);
	void OnBallHitBrick(Entity ball, Entity brick, const CollisionResult& result);
	void OnBallHitHitbox(Entity ball, Entity hitbox, const CollisionResult& result);
	void OnPlayerHitWall(Entity player, Entity wall, const CollisionResult& result);

	// Rendering
	void RenderWalls(const Mesh& mesh, const Material& mat);
	void RenderPrimitiveEntities(const Camera3D& camera);
	void RenderBallShadows(const Mesh& quad, const Material& mat);
	void RenderEndzone(Entity endZone, const Mesh& mesh, const Material& mat);
	void RenderPlayerUI(Entity player);

private:
	EntityManager& m_registry;
	BreakoutFactory& m_factory;

	// Helpers
	Vec3 Reflect(const Vec3& direction, const Vec3& normal) const;
	Vec3 RandomPointInSphere() const;
	void GenerateVectorsInCone(Vector3 direction, float tiltAngle, int n, Vec3 out[]);

};