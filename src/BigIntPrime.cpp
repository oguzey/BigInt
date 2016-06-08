#include "BigInt.h"
#include "logger.h"
#include <assert.h>

#define WORD_BITS	32
#define ROUNDS_MR_TEST 3

void BigInt::generatePrime()
{

	int size = length_ / WORD_BITS;
	std::vector<block> randArray(size);
	RandomGenerator& gen = RandomGeneratorMush::getGeneratorMush();
	int i = 0;

	do {
		do {
			randArray[0] = gen.next32bit();
		} while ((randArray[0] & 1) == 0);
		for (i = 1; i < size; ++i) {
			randArray[i] = gen.next32bit();
		}
		rawArrayToBlocks(randArray);
	} while (!testSimpleDivision() || !testMillerRabin(ROUNDS_MR_TEST, gen, randArray));
}

void BigInt::generateBlumPrime(BigInt &r, BigInt &s)
{
	int size = length_ / WORD_BITS;
	int partSize = size / 2;
	std::vector<block> randArray(size);
	RandomGenerator& gen = RandomGeneratorMush::getGeneratorMush();

	LOG("r part Blum number generating...");
	r.generatePartBlumPrime(gen, randArray, partSize);
	LOG("s part Blum number generating...");
	s.generatePartBlumPrime(gen, randArray, partSize);
//	LOG("r = {}", r.toString());
//	LOG("s = {}", s.toString());
	r.mulHalfNumbers(s, *this);
}

void BigInt::generateBlumPrime()
{
	BigInt r, s;
	generateBlumPrime(r, s);
}

void BigInt::generatePartBlumPrime(RandomGenerator &gen,
				   std::vector<block> &randArray, int partSize)
{
	// need two numbers that are twice smaller than result number
	int size = randArray.size();
	int i = 0;

	assert(partSize < size);

	do {
		do {
			randArray[0] = gen.next32bit();
			//DEBUG("Generate first block of Blum number.");
		} while ((randArray[0] & 3) != 3);
		for (i = 1; i < partSize; ++i) {
			randArray[i] = gen.next32bit();
		}
		for (;i < size; ++i) {
			randArray[i] = 0;
		}
		rawArrayToBlocks(randArray);
	} while (!testSimpleDivision() || !testMillerRabin(ROUNDS_MR_TEST, gen, randArray));
}

bool BigInt::testSimpleDivision()
{
	BigInt y, r, q;
	int simplePrimes[] = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
	int size = sizeof(simplePrimes) / sizeof(simplePrimes[0]);

	// we never get that 2 is divider, because this situation is handled
	// during generation of number
	assert(isEven() == false);

	for (int i = 0; i < size; ++i) {
		y.setNumber(simplePrimes[i]);
		div(y, q, r);
		if (r.isZero()) {
			//LOG("Number has divider {}", simplePrimes[i]);
			return false;
		}
	}
	return true;
}

bool BigInt::isDivisor(BigInt &x)
{
	BigInt r, q;
	div(x, q, r);
	return r.isZero();
}

bool BigInt::testMillerRabin_real(int k, RandomGenerator &gen, std::vector<block> &randArray)
{
	BigInt d, one, x, minusOne, res;//, copyX;

	one.setNumber(1);
	minusOne.copyContent(*this);
	minusOne.sub(one);

	d.copyContent(minusOne);

	block s = 0;
	while (d.getBit(0) == 0) {
		d.shiftRightBit();
		++s;
	}

	for (int i = 0; i < k; ++i) {
		do {
			//LOG("Generare new x...");
			x.generateRand(gen, randArray, posMostSignBit_);
			x.mod(*this);
		} while (x.cmp(1) != 1);

		if (isDivisor(x)) {
//			LOG("Generated x is divisor for p. x = {}, p = {}",
//			    x.toString(), toString());
//			LOG("P is NOT pseudosimple for base x. (1)");
			return false;
		}
		//copyX.copyContent(x);
		x.exp(d, *this, res);
		if (res.isEqual(one) || res.isEqual(minusOne)) {
			//LOG("P is pseudosimple for base x. (2))");
			continue;
		}
		for (block r = 1; r < s; ++r) {
			res.mulMont(res, *this, res);
			if (res.isEqual(one)) {
				//LOG("P is NOT pseudosimple for base x. (3)");
				return false;
			} else if (res.isEqual(minusOne)) {
				//LOG("P is pseudosimple for base x. (3)");
				continue;
			}
		}
		//LOG("After all steps P is NOT pseudosimple for base x. (4)");
		return false;
	}
	return true;
}

bool BigInt::testMillerRabin(int k, RandomGenerator &gen, std::vector<block> &randArray)
{
	initModularReduction();
	bool res = testMillerRabin_real(k, gen, randArray);
	shutDownModularReduction();
	return res;
}
