#include <iostream>
#include "include/BigInt.h"

#define ALLOCATE_LOGGER
#include "logger/logger.h"


int main() {
	//BigInt number1("FFFFFFF093433495732459345382475ACB3984FFFDE98CD");
	BigInt number1("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
			"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
			"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
			"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
			"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
	BigInt number2("1");

	LOG("start value of first number : {}", number1.toString()->c_str());
	LOG("start value of second number : {}", number2.toString()->c_str());
	LOG("--------adding-----------------");
	//number1.add(number2);

	//LOG("result: {}", number1.toString()->c_str());

	//number1.shiftLeft(20);

	//LOG("result of shift left: {}", number1.toString()->c_str());
	//number1.shiftRight(4);
	//LOG("result of shift right1: {}", number1.toString()->c_str());
	//number1.shiftRight(16);
	//LOG("result of shift right1: {}", number1.toString()->c_str());

	BigInt *res = number1.mul(number2, NULL);

	LOG("Mul: number1 = {}", number1.toString()->c_str());
	LOG("Mul: number2 = {}", number2.toString()->c_str());
	LOG("Mul: result = {}", res->toString()->c_str());

	//delete res;

	LOG("All ok.");
	return 0;
}
