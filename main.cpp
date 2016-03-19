#include <iostream>
#include "include/BigInt.h"

#define ALLOCATE_LOGGER
#include "logger/logger.h"


int main() {
	BigInt number1("FFFFFFF093433495732459345382475ACB3984FFFDE98CD");
	BigInt number2("9898faebbadcccc82347234FFfffffFFf90989878768fdecc");

	LOG("start value of first number : {}", number1.toString()->c_str());
	LOG("start value of second number : {}", number2.toString()->c_str());
	LOG("--------adding-----------------");
	number1.add(number2);

	LOG("result: {}", number1.toString()->c_str());

	LOG("All ok.");
	return 0;
}
