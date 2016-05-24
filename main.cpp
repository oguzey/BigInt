#include <iostream>
#define ALLOCATE_LOGGER
#include "logger/logger.h"
#undef ALLOCATE_LOGGER
#include "include/BigInt.h"

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

	LOG("Test Prime number");

	LOG("Prime number = {}", p.toString());

	return 0;
}


void ElGamalSignature()
{
	BigInt p, x, y, g;
	//p.generatePrime();
	p.fromString("FCE34657471693C28AE54ED8A96D06D911E5AE3C262F6CBFAB21EA0121"
		     "958A78B2F77185DCF0F95A7A7402FF73DCC158441EA1BEF3321E6D2939"
		     "2F79696BBBFC27A5E620A72483658D00D4EEC4D7A3BF2774CE8AAB33ED"
		     "AC18056D38EEB1A8CB8AB975B4780729BEE05505606BFCBACCD9A3E33C"
		     "744416AF867C984A1D32DB53");
	p.initModularReduction();

	x.generateRand();
	x.mod(p);

	g.generateRand();
	g.mod(p);



	p.shutDownModularReduction();

}
