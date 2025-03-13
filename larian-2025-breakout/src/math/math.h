#pragma once

// Making this file on Tuesday after sending the email question about
// whether I'm allowed to use GLM or not, for now I'll just make some
// typedefs so I can keep working without knowing

//#include "raylib.h"
//#include "raymath.h"
//
//using Vec2 = Vector2;

#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtx/norm.hpp>

using Vec2 = glm::vec2;
constexpr float EPSILON = glm::epsilon<float>();

#define randf() (float)rand() / RAND_MAX
