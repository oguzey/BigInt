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

int main(int argc, char *argv[])
{
	LOG("Start tests of BigInt implementation...");
	run_test(testConvertToFromString);
	run_test(testSetValues);
	run_test(testAddition);

	LOG("End tests.");
}
