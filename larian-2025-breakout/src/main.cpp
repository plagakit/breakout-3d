#include <iostream>

#include "raylib.h"
#include "ecs/entity_manager.h"
#include "scene/breakout_scene.h"

int main()
{
	std::cout << "Hello World!" << std::endl;

	InitWindow(800, 450, "Larian Interview 2025 :: Breakout :: Thomas Plagakis");
	SetTargetFPS(60);

	EntityManager registry;
	//registry.ReserveEntities(100);
	registry.RegisterComponentType<int>(100);
	for (int i = 0; i < 100; i++)
	{
		Entity e = registry.CreateEntity();
		registry.Add<int>(e, i * 13);
	}
	//	
	//std::cout << "Count: " << registry.Count() << std::endl;
	//for (auto [id, i] : registry.AllWith<int>())
	//{
	//	std::cout << id << " " << i << std::endl;
	//	registry.QueueDelete(id);
	//}
	//registry.FlushDeleteQueue();
	//std::cout << "Count: " << registry.Count() << std::endl;

	BreakoutScene scene;

	while (!WindowShouldClose())
	{
		scene.Update(GetFrameTime());

		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText("Hello World!", 190, 200, 20, LIGHTGRAY);
		scene.Render();
		DrawFPS(0, 0);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}