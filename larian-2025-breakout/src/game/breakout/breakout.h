#pragma once

#include "math/math.h"
#include "ecs/entity_manager.h"
#include "resource/rl_texture.h"
#include "game/breakout/breakout_system.h"
#include "game/breakout/breakout_factory.h"
#include "game/breakout/breakout_components.h"

#include <raylib.h>

// A class for holding all the logic of the breakout game
class Breakout
{
public:
	Breakout(EntityManager& registry);

	void Update(float dt);
	void Render();

private:
	static constexpr size_t RESERVE_SIZE = 1000;
	EntityManager& m_registry;

	BreakoutFactory m_factory;
	BreakoutSystem m_system;

	RLTexture m_txtBall;

	Camera3D m_camera;
	Entity m_player;

};