#pragma once

#include <bitset>
#include <type_traits>

using Entity = uint32_t;
constexpr Entity NULL_ENTITY = 0;

using Signature = uint64_t;
using ComponentID = uint8_t;
constexpr ComponentID MAX_COMPONENT_TYPES = sizeof(Signature) * 8;