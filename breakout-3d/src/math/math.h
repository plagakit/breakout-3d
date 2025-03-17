#pragma once

// I ended up using Raylib just to make it easier
// for compatibility with its functions

#include <raylib.h>
#include <raymath.h>

using Vec2 = Vector2;
using Vec3 = Vector3;

//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm.hpp>
//#include <gtx/norm.hpp>
//
//using Vec2 = glm::vec2;
//using Vec3 = glm::vec3;
//constexpr float EPSILON = glm::epsilon<float>();

#define randf() (float)rand() / RAND_MAX
