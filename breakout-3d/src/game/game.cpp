#include "game.h"

#include <rlgl.h>

Game::Game()
{
	DisableCursor();
	rlEnableDepthTest();
	srand(static_cast<unsigned int>(time(NULL)));

	m_breakout = std::make_unique<Breakout>(m_registry);
}

void Game::Update(float dt)
{
	// Scroll the scrolling background
	//m_scrollBGOffset += SCROLL_BG_VELOCITY * dt;
	//if (m_scrollBGOffset.x > SCROLL_BG_SPACING) m_scrollBGOffset.x -= SCROLL_BG_SPACING;
	//if (m_scrollBGOffset.y > SCROLL_BG_SPACING) m_scrollBGOffset.y -= SCROLL_BG_SPACING;

	m_breakout->Update(dt);

	m_registry.FlushDeleteQueue();
}

void Game::Render()
{
	// Draw scrolling background
	ClearBackground({ 50, 50, 50, 255 });
	//for (int i = 0; i * SCROLL_BG_SPACING < Game::SCREEN_WIDTH; i++)
	//{
	//	int x = static_cast<int>(i * SCROLL_BG_SPACING + m_scrollBGOffset.x);
	//	int y = static_cast<int>(i * SCROLL_BG_SPACING + m_scrollBGOffset.y);

	//	DrawLine(x, -5, x, Game::SCREEN_HEIGHT + 5, { 75, 75, 75, 255 });
	//	DrawLine(-5, y, Game::SCREEN_WIDTH + 5, y, { 75, 75, 75, 255 });
	//}

	m_breakout->Render();
}
