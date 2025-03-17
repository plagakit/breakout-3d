#include "brick_generation.h"

#include <vector>

BrickGenerator::BrickGenerator() 
{
	m_eligibleTypes.reserve(Brick::TYPE_SIZE);
	for (int i = 0; i < Brick::TYPE_SIZE; i++)
		m_counts[i] = 0;
}

void BrickGenerator::Reset()
{
	m_eligibleTypes.clear();
	for (int i = 0; i < Brick::TYPE_SIZE; i++)
		m_counts[i] = 0;
}

Brick::Type BrickGenerator::Next(const BrickGridGenerationParams& params)
{
	// Filter eligible types and calculate weights
	float totalDefinedWeight = 0.0f;
	int undefinedChances = 0;

	for (const auto& config : params.typeConfigs)
	{
		// Skip if max count reached
		if (config.maxCount != -1 && m_counts[config.type] >= config.maxCount)
			continue;

		// Calculate chance
		if (config.spawnChance < 0.0f) // rest of the weight
			undefinedChances++;
		else
			totalDefinedWeight += config.spawnChance;

		m_eligibleTypes.push_back(config);
	}

	const float remainingWeight = 1.0f - totalDefinedWeight;
	const float remainingEach = undefinedChances > 0 ?
		remainingWeight / undefinedChances : 0.0f;

	// Generate random values 
	// for each brick type, we add its weight as a cutoff
	// ex. grav = 0.1, curve = 0.1, if RV = 0.15 we check
	//     0 < rv < 0.1 which has 10% chance, since it isnt
	//     accum becomes 0.1 + 0.1 = 0.2, and we check
	//     0.1 < rv < 0.2 which still has 10% chance, and so
	//     that brick becomes grav

	float cutoff = 0.0f;
	float rv = randf();
	for (const auto& config : m_eligibleTypes)
	{
		float weight = config.spawnChance < 0.0f ?
			remainingEach : config.spawnChance;

		cutoff += weight; 
		if (rv < cutoff)
		{
			m_counts[config.type]++;
			return config.type;
		}
	}

	return Brick::NORMAL;
}
