#pragma once

#include "math/math.h"
#include "ecs/entity_manager.h"
#include "resource/rl_texture.h"
#include "game/breakout/systems/breakout_system.h"
#include "game/breakout/factories/breakout_factory.h"
#include "game/breakout/factories/brick_generation.h"
#include "game/breakout/components/breakout_components.h"

#include <raylib.h>

// A class for orchestrating all the logic of the breakout game
class Breakout
{
public:
	Breakout(EntityManager& registry);
	~Breakout();

	void Update(float dt);
	void Render();

	void RestartGame();

private:
	// Essentials
	EntityManager& m_registry;

	BreakoutFactory m_factory;
	BreakoutSystem m_system;

	// Resources
	// I would have like to include better resource management maybe
	// using RAII or some sort of resource manager but I tried focusing
	// on gameplay features instead

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

	static constexpr int DEFAULT_LIVES = 3;
	static constexpr Vec3 PLAYER_START_POS = { 0.0f, 1.5f, 0.0f };
	static constexpr Vec3 BALL_START_POS = { 0.0f, 5.0f, 3.0f };
	static constexpr float ROUND_START_TRANS_TIME = 2.0f;
	static constexpr float ROUND_END_TRANS_TIME = 1.0f;

	// Instead of making this a const embedded into the game we can
	// load it from a JSON, pass it into Breakout as a param from Game, etc.
	// but for the small scope of this project, this works
	static constexpr BrickTypeInfoArr BRICK_TYPE_INFO = {
		BrickTypeInfo{ 100, PURPLE, DARKPURPLE },			// NORMAL	
		BrickTypeInfo{ 150, RED, MAROON },					// CURVE	
		BrickTypeInfo{ 200, BLUE, DARKBLUE },				// GRAVITY
		BrickTypeInfo{ 150, { 85, 212, 110, 255 }, GREEN }	// ADD_BALLS
	};

	enum GameState
	{
		HALTING,			// for when the game is paused
		ROUND_START_TRANS,	// 2-second anim where ball flashes and player respawns
		PLAYING,			// main state
		ROUND_END_TRANS,	// 1-second camera transition back to spawn location
	};
	GameState m_gameState;
	float m_transitionTimer;

	Vec3 m_oldCameraPos;
	Vec3 m_oldTargetPos;

	int m_points;
	int m_lives;
	BrickGridGenerationParams m_gridGenParams;

	Camera3D m_camera;
	Entity m_player;
	Entity m_endZone;
	Entity m_firstBall;

	void StartRound();
	void EndRound();
	void WinRound();
	void LoseLife();

};