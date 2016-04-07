#include <string>
#include <string.h>
#include <array>
#include <assert.h>
#include "../include/BigInt.h"

#define ALLOCATE_LOGGER
#include "../logger/logger.h"

#define GREEN	"\033[1;32m"
#define YELLOW	"\033[1;33m"
#define RED	"\033[1;31m"
#define RESET_COLOR "\033[0m"
#define PAINT(word, color) color word RESET_COLOR

#define assertStrMsg(str1, str2, msg)						\
do {										\
	std::string a_str1 = str1;					\
	std::string a_str2 = str2;					\
	if (a_str1 != a_str2) {						\
		INFO("Str '{}' is not equal to str '{}'", a_str1, a_str2);	\
		CRITICAL(PAINT("[{}:{}] " msg, RED), __func__, __LINE__);	\
	}									\
} while (0)


#define assertMsg(cond, msg)							\
	if (!(cond)) {								\
		CRITICAL(PAINT("[{}:{}] " msg, RED), __func__, __LINE__);	\
	}

#define run_test(test_func)			\
do {						\
	INFO(PAINT("Start of test "#test_func, YELLOW));	\
	test_func();				\
	INFO(PAINT("End of test "#test_func, YELLOW));	\
} while (0)


void testConvertToFromString()
{
	std::array<char, 16> hexChars = {{'0', '1', '2', '3', '4', '5', '6', '7',
					'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}};
	std::string str;
	for (int i = 0; i < 256; ++i) {
		str.push_back(hexChars[i % 16]);
	}
	BigInt a(str);
	std::string new_str = a.toString();
	assertMsg(str == new_str, "String is not equals.");
}

void testSetValues()
{
	BigInt number;
	std::string numberStr;

	number.setZero();
	numberStr.assign(256, '0');
	assertStrMsg(number.toString(), numberStr, "Zero number fail.");

	number.setNumber(1);
	numberStr.assign(255, '0');
	numberStr.push_back('1');
	assertStrMsg(number.toString(), numberStr, "Set custom number fail.");

	number.setMax();
	numberStr.assign(256, 'F');
	assertStrMsg(number.toString(),  numberStr, "Max number fail");

	number.setZero();
	assertMsg(number.isZero(), "Number is not zero.");

	number.setNumber(1);
	assertMsg(number.isZero() == false, "Number is zero.");

}

void testAddition()
{
	BigInt a;
	BigInt b;
	BigInt zero;
	zero.setZero();

	a.setZero();
	b.setZero();
	a.add(b);
	assertMsg(a.isEqual(zero), "Adding zero failed.");

	a.setMax();
	b.setNumber(1);
	a.add(b);
	assertMsg(a.isEqual(zero), "Adding max value failed.");

	std::string str;
	str.assign(256, '8');
	a.fromString(str);
	str.assign(256, '7');
	b.fromString(str);
	a.add(b);
	b.setMax();
	assertMsg(a.isEqual(b), "Adding numbers failed.");

	a.setMax();
	b.setNumber(1);
	a.add(b);
	zero.setZero();
	assertMsg(a.isEqual(zero), "Overflow number during addition failed.");
}

void testSubtraction()
{
	BigInt a;
	BigInt b;
	BigInt zero;
	zero.setZero();

	a.setZero();
	b.setZero();
	a.sub(b);
	assertMsg(a.isZero(), "Number is not zero.");

	b.setNumber(1);
	a.sub(b);
	b.setMax();
	assertMsg(a.isEqual(b), "Numbers is not equals.");

	a.setNumber(1);
	b.setZero();
	a.sub(b);
	b.setNumber(1);
	assertMsg(a.isEqual(b), "Number is not one.");

	std::string str;
	str.assign(256, '8');
	a.fromString(str);
	str.assign(256, '7');
	b.fromString(str);
	a.sub(b);

	str.assign(256, '1');
	b.fromString(str);
	assertMsg(a.isEqual(b), "Subtracting numbers failed.");
}

void testCmp()
{
	BigInt a, b;

	a.setZero();
	b.setNumber(255);
	assertMsg(a.cmp(b) == -1, "Zero greater than 255.");
	assertMsg(b.cmp(a) == 1, "255 less than zero.");

	a.setNumber(255);
	assertMsg(a.cmp(b) == 0, "255 != 255.");
	assertMsg(b.cmp(a) == 0, "255 != 255 (2)");
}

void testBits()
{
	BigInt a, b;
	unsigned int len = a.getLength();
	unsigned int i;

	for (i = 0; i < len; ++i) {
		a.setBit(i, 1);
	}
	b.setMax();
	assertMsg(a.isEqual(b), "Number has not max value.");

	for (i = 0; i < len; ++i) {
		assertMsg(a.getBit(i) == 1, "Bit is not one.")
	}

	for (i = len - 1; i > 0; --i) {
		a.setBit(i, 0);
	}
	b.setNumber(1);
	assertMsg(a.isEqual(b), "Number has not one.");
	a.setBit(0, 0);
	assertMsg(a.isZero(), "Number is not zero.");

	for (i = 0; i < len; ++i) {
		assertMsg(a.getBit(i) == 0, "Bit is not zero.")
	}
}

void testShiftLeft()
{
	BigInt a, b;

	b.setMax();
	a.setNumber(1);
	b.sub(a);

	a.setMax();
	a.shiftLeft(1);
	assertMsg(a.isEqual(b), "Shift to 1 is failed.");

	a.shiftLeft(3);

	std::string numberStr;
	numberStr.assign(255, 'F');
	numberStr.push_back('0');
	b.fromString(numberStr);
	assertMsg(a.isEqual(b), "Shift to 4 is failed.");

	a.shiftLeft(a.getLength() - 4);
	b.setZero();
	assertMsg(a.isEqual(b), "Shift to 1024 is failed.");
}

void testShiftRight()
{
	BigInt a, b;

	b.setMax();
	a.setNumber(1);
	a.shiftLeft(a.getLength() - 1);
	b.sub(a);

	a.setMax();
	a.shiftRight(1);
	assertMsg(a.isEqual(b), "Shift right 1 failed.");

	a.shiftRight(a.getLength() - 2);
	b.setNumber(1);
	assertMsg(a.isEqual(b), "Shift right 1023 failed.");

	a.shiftRight(1);
	assertMsg(a.isZero(), "Shift right 1024 failed.");
}

void testMultiplication()
{
	BigInt a, b;
	BigInt *res = NULL;
	BigInt *res2 = NULL;

	a.setMax();
	b.setZero();
	res = a.mul(b, &res2);
	assertMsg(res != NULL, "Result of multiplication failed.");
	assertMsg(res2 == res, "Pass output of multiplication throuth parameters failed.");

	assertMsg(res->isZero(), "Multiplication by zero failed.");
	delete res;
}

int main(int argc, char *argv[])
{
	LOG("Start tests of BigInt implementation...");

	run_test(testConvertToFromString);
	run_test(testSetValues);
	run_test(testAddition);
	run_test(testSubtraction);
	run_test(testMultiplication);
	run_test(testShiftLeft);
	run_test(testShiftRight);
	run_test(testCmp);
	run_test(testBits);

	LOG("End tests.");
}
