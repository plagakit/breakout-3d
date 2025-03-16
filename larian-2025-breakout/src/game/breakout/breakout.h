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
	~Breakout();

	void Update(float dt);
	void Render();

private:
	static constexpr size_t RESERVE_SIZE = 1000;
	EntityManager& m_registry;

	BreakoutFactory m_factory;
	BreakoutSystem m_system;

	//RLTexture m_txtBall;

	Texture m_txChecker;
	Texture m_txEndzone;
	Mesh m_meshPlane;
	Material m_matPlane;
	Material m_matEndzone;

	Texture m_txBallShadow;
	Mesh m_meshBallShadow;
	Material m_matBallShadow;

	Camera3D m_camera;
	Entity m_player;
	Entity m_endZone;

	static constexpr float ROOM_WIDTH = 15.0f;
	static constexpr float ROOM_HEIGHT = 12.0f;
	static constexpr float ROOM_LENGTH = 25.0f;
	static constexpr float END_ZONE = 24.0f;

};