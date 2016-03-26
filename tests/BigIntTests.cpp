#include <string>
#include <array>
#include <assert.h>
#include "../include/BigInt.h"

#define ALLOCATE_LOGGER
#include "../logger/logger.h"


#define assert_msg(cond, msg)					\
	if (!(cond)) {						\
		CRITICAL("[%s:%d] " msg, __func__, __LINE__);	\
	}

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
	assert_msg(str == new_str, "String is not equals.");
}

void testSetValues()
{
	BigInt number;
	std::string number_str;

	number.setZero();
	number_str.resize(256, '0');
	assert_msg(number.toString() == number_str, "Zero number fail.");

	number.setMax();
	number_str.resize(256, 'F');
	assert_msg(number.toString() == number_str, "Max number fail");

	number.setNumber(1);
	number_str.resize(255, '0');
	number_str.push_back('1');
	assert_msg(number.toString() == number_str, "Set custom number fail.");
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
	assert_msg(a.isEqual(zero), "Adding zero failed.");

	a.setMax();
	b.setNumber(1);
	a.add(b);
	assert_msg(a.isEqual(zero), "Adding max value failed.");

	std::string str;
	str.resize(256, '8');
	a.fromString(str);
	str.resize(256, '7');
	b.fromString(str);
	a.add(b);
	b.setMax();
	assert_msg(a.isEqual(b), "Adding numbers failed.");
}

int main(int argc, char *argv[])
{
	LOG("Start tests of BigInt implementation...");
	testConvertToFromString();
	testSetValues();
	testAddition();

	LOG("End tests.");
}
