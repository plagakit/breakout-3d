# Breakout 3D

Controls:

- WASD to move horizontally
- Space to jump while on the ground
- Left shift to dash in the direction you're moving
- Left click to attack and deflect any balls you hit in the direction you're facing

This game is Breakout but 3D, inspired by games like Deathball on ROBLOX. I reused ECS code from my Ubisoft NEXT 2025 project, specifically everything under the `ecs` folder, the source for which can be found [here](https://github.com/plagakit/ubisoft-next-2025/tree/main/Engine/src/entity/entity_manager). If I had more time, I'd implement more features. I can talk about them and how I think
I'd implement them here:

- Powerups

  - New component type, systems detect Player vs. Powerup collision
  - Powerups are added to an inventory
  - Extendo Arm powerup:
    - A homing PlayerHitbox that starts from the player pos, moves towards the closest ball, and hits it with a return direction of the furthest block

- Roguelike features
  - I built `Game` on top of `Breakout` for the purpose of making a roguelike-type game - when the player clears a grid of bricks, they get a new item and the brick configuration gets more difficult.
  - Also why I tried making most variables in `Player` like `speed` or `dashCooldown` mutable
  - Didn't have time to implement this because I pivoted from a 2D game to a 3D in the middle of the week
- Design based on data, ex. most of the numbers and stats within JSON files instead of hard-coded as constexpr variables
