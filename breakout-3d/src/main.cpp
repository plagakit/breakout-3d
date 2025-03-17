#include <iostream>

#include "raylib.h"
#include "game/game.h"

int main()
{
	std::cout << "Hello World!" << std::endl;

	InitWindow(Game::SCREEN_WIDTH, Game::SCREEN_HEIGHT, Game::SCREEN_TITLE);
	SetTargetFPS(60);

	//EntityManager registry;
	//registry.ReserveEntities(100);
	//registry.RegisterComponentType<int>(100);
	//for (int i = 0; i < 100; i++)
	//{
	//	Entity e = registry.CreateEntity();
	//	registry.Add<int>(e, i * 13);
	//}
	//	
	//std::cout << "Count: " << registry.Count() << std::endl;
	//for (auto [id, i] : registry.AllWith<int>())
	//{
	//	std::cout << id << " " << i << std::endl;
	//	registry.QueueDelete(id);
	//}
	//registry.FlushDeleteQueue();
	//std::cout << "Count: " << registry.Count() << std::endl;

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