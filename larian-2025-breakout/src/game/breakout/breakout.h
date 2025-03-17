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

	void StartGame();
	void LoseGame();
	void WinGame();

private:
	// Essentials
	EntityManager& m_registry;

	BreakoutFactory m_factory;
	BreakoutSystem m_system;

	// Resources

	Texture m_txChecker;
	Texture m_txEndzone;
	Mesh m_meshPlane;
	Material m_matPlane;
	Material m_matEndzone;

	Texture m_txBallShadow;
	Mesh m_meshBallShadow;
	Material m_matBallShadow;

	// Game objects & logic

	static constexpr float ROOM_WIDTH = 15.0f;
	static constexpr float ROOM_HEIGHT = 12.0f;
	static constexpr float ROOM_LENGTH = 32.0f;
	static constexpr float END_ZONE = 30.0f;

	static constexpr Vec3 PLAYER_START_POS = { 0.0f, 0.5f, 0.0f };
	static constexpr Vec3 BALL_START_POS = { 0.0f, 3.0f, 3.0f };
	static constexpr float BALL_RADIUS = 0.5f;
	static constexpr float START_GAME_TRANS_TIME = 1.0f;
	static constexpr float END_GAME_TRANS_TIME = 2.0f;

	enum GameState
	{
		HALTING,				// for when the game is paused
		START_GAME_TRANSITION,	// 2-second anim where ball flashes and player respawns
		PLAYING,				// main state
		END_GAME_TRANSITION,	// 2-second camera transition back to spawn location
	};
	GameState m_gameState;
	float m_transitionTimer;

	int m_points;

	Camera3D m_camera;
	Entity m_player;
	Entity m_endZone;

};