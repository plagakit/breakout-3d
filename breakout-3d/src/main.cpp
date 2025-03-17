#include <iostream>

#include "raylib.h"
#include "game/game.h"

int main()
{
	std::cout << "Hello World!" << std::endl;

	InitWindow(Game::SCREEN_WIDTH, Game::SCREEN_HEIGHT, Game::SCREEN_TITLE);
	SetTargetFPS(60);

	Game game;

	while (!WindowShouldClose())
	{
		game.Update(GetFrameTime());

		BeginDrawing();
		ClearBackground(BLACK);
		game.Render();
#ifdef _DEBUG
		DrawFPS(0, 0);
#endif
		EndDrawing();
	}

	CloseWindow();

	return 0;
}