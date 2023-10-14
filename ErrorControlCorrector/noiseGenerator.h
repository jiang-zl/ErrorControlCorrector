#pragma once
#ifndef ECC_NOISE_GENERATOR_H
#define ECC_NOISE_GENERATOR_H

#include <bitset>
#include <random>
#include <ctime>


class NoiseGenerator
{
private:
	static std::mt19937 eng;

public:
	template<size_t _NBits, size_t _KBits>
	static size_t setNoise(DataFrame<_NBits, _KBits>& df)
	{
		std::uniform_int_distribution<size_t> uni(0, _NBits - 1);
		size_t retIndex = uni(NoiseGenerator::eng);
		df.m_dataFrame.flip(retIndex);
		return retIndex;
	}
};

std::mt19937 NoiseGenerator::eng(std::random_device{}());

#endif // !ECC_NOISE_GENERATOR_H
