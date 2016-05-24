#include "BigInt.h"
#include "logger.h"
#include <assert.h>
#include "GeneratorMush.h"

#define WORD_BITS	32

void BigInt::generatePrime()
{

	int size = length_ / WORD_BITS;
	std::vector<block> randArray(size);
	GeneratorMush& gen = GeneratorMush::getGeneratorMush();
	int i = 0;

	do {
		for (i = 0; i < size; ++i) {
			randArray[i] = gen.next32bit();
		}
		rawArrayToBlocks(randArray);
	} while (!testSimpleDivision() || !testMillerRabin(3));
}

bool BigInt::testSimpleDivision()
{
	BigInt y, r, q;

	int simplePrimes[] = {3, 5, 7, 11, 13, 17};
	int size = sizeof(simplePrimes) / sizeof(simplePrimes[0]);
	assert(size == 6);

	if (getBit(0) == 0) {
		// 2 is divider
		return false;
	}

	for (int i = 0; i < size; ++i) {
		y.setNumber(simplePrimes[i]);
		div(y, q, r);
		if (r.isZero()) {
			LOG("Number has divider {}", simplePrimes[i]);
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

bool BigInt::testMillerRabin_real(int k)
{
	BigInt d, one, x, minusOne, res, copyX;

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
			LOG("Generare new x...");
			x.generateRand();
			x.mod(*this);
		} while (x.cmp(1) != 1);

		if (isDivisor(x)) {
			LOG("Generated x is divisor for p. x = {}, p = {}",
			    x.toString(), toString());
			LOG("P is NOT pseudosimple for base x. (1)");
			return false;
		}
		copyX.copyContent(x);
		x.exp(d, *this, res);
		if (res.isEqual(one) || res.isEqual(minusOne)) {
			LOG("P is pseudosimple for base x. (2))");
			continue;
		}
		for (block r = 1; r < s; ++r) {
			res.mulMont(res, *this, res);
			if (res.isEqual(one)) {
				LOG("P is NOT pseudosimple for base x. (3)");
				return false;
			} else if (res.isEqual(minusOne)) {
				LOG("P is pseudosimple for base x. (3)");
				continue;
			}
		}
		LOG("After all steps P is NOT pseudosimple for base x. (4)");
		return false;
	}
	return true;
}

bool BigInt::testMillerRabin(int k)
{
	initModularReduction();
	bool res = testMillerRabin_real(k);
	shutDownModularReduction();
	return res;
}
