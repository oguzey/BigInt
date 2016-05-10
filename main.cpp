#include <iostream>
#include "include/BigInt.h"

#define ALLOCATE_LOGGER
#include "logger/logger.h"


int main() {
	// create default BigInt
	BigInt biDefault;

	// create BigInt from hex string
	BigInt biNumber("AAAFFF2428348");

	// example of addition of two numbers
	// result will be written in first number

	BigInt a, b;

	a.setZero();
	b.setNumber(1);
	a.add(b);

	// example of multiplication of two numbers with modular reduction

	BigInt x, y, m, ret;

	x.fromString("4cd");
	y.fromString("16a0");
	m.fromString("11bbf");

	m.initModularReduction();
	x.mulMont(y, m, ret);
	m.shutDownModularReduction();

	LOG("Result of multiplication is {}", ret.toString());

	return 0;
}
