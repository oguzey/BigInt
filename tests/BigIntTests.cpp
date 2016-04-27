#include <string>
#include <string.h>
#include <array>
#include <assert.h>
#include <ctime>
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

#define assertEqualMsg(arg1, arg2, msg)						\
	if (arg1 != arg2) {							\
		CRITICAL(PAINT("[{}:{}] Arguments '{}' and '{}' not equals. " msg, RED), __func__, __LINE__, arg1, arg2);	\
	}

#define assertMsg(cond, msg)							\
	if (!(cond)) {								\
		CRITICAL(PAINT("[{}:{}] " msg, RED), __func__, __LINE__);	\
	}

#define runTest(test_func)					\
do {								\
	INFO(PAINT("Start of test "#test_func, YELLOW));	\
	test_func();						\
	INFO(PAINT("End of test "#test_func, YELLOW));		\
} while (0)

inline double mesureTime(void (*function)(void))
{
	std::clock_t begin = std::clock();
	function();
	std::clock_t end = std::clock();
	return double(end - begin) / CLOCKS_PER_SEC;
}

#define mesureTimeRunning(func)						\
do {									\
	INFO(PAINT("Start running the function "#func, YELLOW));	\
	std::clock_t begin = std::clock();				\
	func();								\
	std::clock_t end = std::clock();				\
	INFO(PAINT("End running the function "#func, YELLOW));		\
	INFO(PAINT("Time of running {:.10f}", YELLOW),			\
			double(end - begin) / CLOCKS_PER_SEC);		\
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

	a.setMax();
	for (i = len - 1; i > 0; --i) {
		assertEqualMsg(1, a.clearBit(i), "Clear bit fail.");
	}

	b.setNumber(1);
	assertMsg(a.isEqual(b), "After clear bit should be one.");

	assertEqualMsg(1, a.clearBit(0), "Clear bit fail.");
	assertMsg(a.isZero(), "Is zero fail.")
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

	a.setNumber(1);
	a.shiftRightBlock(0);
	assertMsg(a.isEqual(b), "Shift right 0 failed.");

}

void testIsEqual()
{
	BigInt a, b;
	a.setMax();
	b.setMax();
	assertMsg(a.isEqual(b), "IsEqual with equal len of numbers was failed.");

	BigInt *res = BigInt::getDoubleNumber();
	a.setMax();
	res->copyContent(a);

	assertMsg(a.isEqual(*res), "IsEqual(t) with greater number was failed.");
	assertMsg(res->isEqual(a), "IsEqual(t) with less number was failed.");

	res->shiftLeftBlock(29);
	for (int i = 0; i < 29; ++i) {
		res->setBit(i, 1);
	}

	assertMsg(a.isEqual(*res) == false, "IsEqual(f) with greater number was failed.");
	assertMsg(res->isEqual(a) == false, "IsEqual(f) with less number was failed.");

	delete res;

}

void testMontgomeryMultiplication()
{
	BigInt x, y, m, check, ret;

	x.setNumber(3);
	y.setNumber(5);
	m.setNumber(17);
	m.initModularReduction();
	x.mulMont(y, m, ret);
	m.shutDownModularReduction();
	check.setNumber(15);
	assertMsg(ret.isEqual(check), "Fail mont mul in simple case.");

	x.fromString("4cd");
	y.fromString("16a0");
	m.fromString("11bbf");
	check.setNumber(0x11AC1);

	m.initModularReduction();
	x.mulMont(y, m, ret);
	m.shutDownModularReduction();
	assertMsg(ret.isEqual(check), "Fail mont mul in midle case.");

	x.fromString("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
		     "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
		     "aaaaaaaaaaaaaaaaaaaaaaa");
	y.fromString("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaFFFFFFFFFF"
		     "Ffffffffffffffffffffffffffffffff1231723617231218238899798797a979a8a9"
		     "7a97a987aa78a798797979");
	m.fromString("bbbbbbbbbddbbcbdbdbcdcbdbcdbcbbdfffffff1231723617231218238899798797a"
		     "979a8a97a97a987aa78a798797979798cd8c7d87cd987c8d7c9d7c97d9c7d8748236"
		     "4827368476238468273648273648263641041209809423942091");
	check.fromString("A4E23E498064BFB3CC0D5F510A9D94BD062BECE7ED22DAE3C9ED62336E106D64"
			 "EABF956A92A94710B96112F2955BE4D87DB247F525E637CBA627B337B28EC50B"
			 "F86499DF8E0E02DDE203D9E237F213FE865699021B87553412878A427811");

	m.initModularReduction();
	x.mulMont(y, m, ret);
	m.shutDownModularReduction();
	assertMsg(ret.isEqual(check), "Fail mont mul in large case.");

	x.fromString("4B");
	y.fromString("4B");
	m.fromString("6D");

	m.initModularReduction();
	x.mulMont(y, m, ret);
	m.shutDownModularReduction();

	check.fromString("42");
	assertMsg(ret.isEqual(check), "Fail mont mul in squaring case.");

}

void testGetPosMostSignificatnBit()
{
	BigInt a;
	int len = a.getLength();

	a.setZero();

	assertEqualMsg(-1, a.getPosMostSignificatnBit(), "Fail for zero");

	a.setNumber(1);
	assertEqualMsg(0, a.getPosMostSignificatnBit(), "Fail for one");

	a.setZero();
	for (int i = 0; i < len; ++i) {
		a.setBit(i, 1);
		assertEqualMsg(i, a.getPosMostSignificatnBit(), "Fail for loop");
	}
}

void testMultiplicationByBit()
{
	BigInt a, b;

	b.setMax();
	a.setMax();
	a.mulByBit(1);
	assertMsg(a.isEqual(b), "Fail during mul by one.");

	a.mulByBit(0);
	assertMsg(a.isZero(), "Fail during mul by zero.");
}

void testModularReduction()
{
	BigInt check;
	BigInt a("f7b15cdf");
	BigInt m("11bbf");

	m.initModularReduction();

	a.mod(m);
	check.fromString("FA57");
	assertMsg(a.isEqual(check), "Mod fail.");

	a.fromString("34fd");
	a.mod(m);
	check.fromString("34FD");
	assertMsg(a.isEqual(check), "Mod fail 2.");

	a.fromString("134fd");
	a.mod(m);
	check.fromString("193E");
	assertMsg(a.isEqual(check), "Mod fail 3.");

	m.shutDownModularReduction();
}

void testCopy()
{
	BigInt a, b;

	a.setBit(354, 1);
	b.copyContent(a);
	assertEqualMsg(1, b.getBit(354), "Fail during copy content");

	a.setMax();
	b.copyContent(a);
	assertMsg(b.isEqual(a), "Fail copy content.");
}

void testExp()
{
	BigInt a, e, m, ret;
	BigInt one;
	one.setNumber(1);

	m.fromString("DE5BF25EFA23FE78BD634DFB6AFD49AEDFF7CF41CE4390F49E6D1408BC"
		     "95A48FF1FFC7F91F45E220484F04D840BF00A75E5AC8B0BE5EA946AC52"
		     "77863B34129B0AEE65548967413C777B691156E3CE5020DE44BF3B526E"
		     "5AF879561E4717E6518889363D84A33BE1B87C786089DEB514ED9ADAB3"
		     "45B819D22DDA9E4E004C772D");
	e.copyContent(m);
	a.setNumber(1);
	e.sub(a);  // e = m - 1

	a.fromString("AAAAAFF");

	m.initModularReduction();
	a.exp(e, m, ret);
	m.shutDownModularReduction();
	assertMsg(ret.isEqual(one), "Fail of test Ferma");

	a.fromString("5AC");
	m.fromString("6D");
	e.fromString("6C");
	m.initModularReduction();
	a.exp(e, m, ret);
	m.shutDownModularReduction();
	assertMsg(ret.isEqual(one), "Fail of test Ferma");

}


void mulBitByOne()
{
	BigInt num;
	num.setMax();
	for (int i = 0; i < 10000; ++i) {
		num.mulByBit(1);
	}
}

void mulBitByZero()
{
	BigInt num;
	num.setMax();
	for (int i = 0; i < 10000; ++i) {
		num.mulByBit(0);
	}
}

int main(int argc, char *argv[])
{
	LOG("Start tests of BigInt implementation...");

	runTest(testIsEqual);
	runTest(testConvertToFromString);
	runTest(testSetValues);
	runTest(testAddition);
	runTest(testSubtraction);
	runTest(testShiftLeft);
	runTest(testShiftRight);
	runTest(testCmp);
	runTest(testBits);
	runTest(testGetPosMostSignificatnBit);
	runTest(testMontgomeryMultiplication);
	runTest(testMultiplicationByBit);
	runTest(testModularReduction);
	runTest(testCopy);
	runTest(testExp);

	mesureTimeRunning(mulBitByOne);
	mesureTimeRunning(mulBitByZero);
	mesureTimeRunning(testExp);


	LOG("End tests.");
}
