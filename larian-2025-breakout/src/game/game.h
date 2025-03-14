#pragma once

#include "math/math.h"
#include "ecs/entity_manager.h"
#include "breakout/breakout.h"

#include <raylib.h>
#include <raymath.h>

/**
The main game that orchestrates everything - breakout, and the roguelike
that is built on top of it. Also holds essential systems like the entity
registry.
*/
class Game
{
public:
	static constexpr uint32_t SCREEN_WIDTH = 1152;
	static constexpr uint32_t SCREEN_HEIGHT = 648;
	static constexpr const char* SCREEN_TITLE = "Larian Interview 2025 :: Breakout :: Thomas Plagakis";

	Game();
	void Update(float dt);
	void Render();
	~Game() = default;

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

private:
	EntityManager m_registry;

	std::unique_ptr<Breakout> m_breakout;

	// Scrolling background
	static constexpr float SCROLL_BG_SPD = 50.0f;
	static constexpr float SCROLL_BG_SPACING = 25.0f;
	Vec2 SCROLL_BG_VELOCITY = { SCROLL_BG_SPD, SCROLL_BG_SPD };

	Vec2 m_scrollBGOffset = { 0.0f, 0.0f };

	Image checked;

};