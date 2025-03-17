#pragma once

#include "game/breakout/components/breakout_components.h"

#include <raylib.h>
#include <array>
#include <vector>

// Config and info data for each brick type

struct BrickTypeInfo
{
	int points;
	Color color;
	Color outlineColor;
};

using BrickTypeInfoArr = std::array<BrickTypeInfo, Brick::TYPE_SIZE>;

struct BrickTypeGenConfig
{
	Brick::Type type;

	float spawnChance;	// -1 for the rest of the weight
	int maxCount;		// -1 for unlimited
};

using BrickTypeGenConfigArr = std::array<BrickTypeGenConfig, Brick::TYPE_SIZE>;

struct BrickGridGenerationParams
{
	Vec3 slices = { 1, 1, 1 };
	Vec3 spacing = { 0.5f, 0.5f, 0.5f };

	uint8_t health = 1;

	BrickTypeGenConfigArr typeConfigs = {
		BrickTypeGenConfig{ Brick::NORMAL, -1.0f, -1  },
		BrickTypeGenConfig{ Brick::GRAVITY, 0.1f, 3   },
		BrickTypeGenConfig{ Brick::CURVE, 0.15f, -1    },
		BrickTypeGenConfig{ Brick::ADD_BALLS, 0.15f, 2 }
	};
};

// A helper class for randomly generating bricks in a grid while conforming
// to the chance and max brick params in the config
class BrickGenerator
{
public:
	BrickGenerator();
	void Reset();
	Brick::Type Next(const BrickGridGenerationParams& params);

private:
	std::array<int, Brick::TYPE_SIZE> m_counts;
	std::vector<BrickTypeGenConfig> m_eligibleTypes;

};