#ifndef GENERATORMUSH_H
#define GENERATORMUSH_H

#include <cstdlib>
#include <ctime>

#include <array>
#include <cstdint>
#include <climits>
#include <random>
#include "../logger/logger.h"

static_assert(sizeof(unsigned int) == 4, "Support only 32-bit of integer");
static_assert(sizeof(int64_t) == 8, "Need 64-bit integer for generator");

class GeneratorMush
{
public:
	static GeneratorMush& getGeneratorMush()
	{
		LOG("Get generator");
		static GeneratorMush generator;
		return generator;
	}

	~GeneratorMush() {LOG("New generator was destroyed.");}
	GeneratorMush(GeneratorMush const&) = delete;
	void operator=(GeneratorMush const&) = delete;

	unsigned int next32bit()
	{
		if (overflowA) {
			runSubGeneratorB();
		}
		if (overflowB) {
			runSubGeneratorA();
		}
		runSubGeneratorA();
		runSubGeneratorB();
		return A[sizeA - 1] ^ B[sizeB - 1];
	}

private:
	/*
	 * A[i] = (A[i - 55] + A[i - 24]) mod 2^32
	 * B[i] = (B[i - 52] + B[i - 19]) mod 2^32
	 */
	const int sizeA = 55;
	const int sizeB = 52;
	std::array<unsigned int, 55> A;
	std::array<unsigned int, 52> B;
	bool overflowA;
	bool overflowB;

	GeneratorMush()
	{
		std::mt19937 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, UINT_MAX);

		int i;

		for (i = 0; i < sizeA; ++i) {
			A[i] = dist(rng);
		}

		for (i = 0; i < sizeB; ++i) {
			B[i] = dist(rng);
		}
		overflowA = false;
		overflowB = false;
		LOG("New generator was created.");
	}

	void runSubGeneratorA()
	{
		uint64_t A55 = (uint64_t)A[55 - 55] + (uint64_t)A[55 - 24];

		overflowA = A55 > UINT_MAX;

		for (int i = 0; i < sizeA - 1; ++i) {
			A[i] = A[i + 1];
		}
		A[sizeA - 1] = A55;
	}

	void runSubGeneratorB()
	{
		uint64_t B52 = (uint64_t)B[52 - 52] + (uint64_t)B[52 - 19];

		overflowB = B52 > UINT_MAX;

		for (int i = 0; i < sizeB - 1; ++i) {
			B[i] = B[i + 1];
		}
		B[sizeB - 1] = B52;
	}
};

#endif // GENERATORMUSH_H
