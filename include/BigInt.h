#ifndef BIG_INT_H
#define BIG_INT_H
#include <iostream>
#include <string>
#include <string.h>
#include <array>

static_assert(sizeof(unsigned int) == 4, "Support only 32-bit of integer");


class BigInt {
public:
	BigInt();
//	BigInt(unsigned int number);
	BigInt(const char *strHexNumber);
	~BigInt();
	BigInt* add(BigInt &a, BigInt &b);
	int fromString(const char *hexString);
	int fromString(const std::string &hexString);
	std::string* toString();

private:
	unsigned int *blocks_;
	unsigned int length_;
	unsigned int size_;

	int hexCharToInteger(char digit);
	void rawArrayToBlocks(std::array<unsigned int, 32> &rawArray);
	void blocksToRawArray(std::array<unsigned int, 32> &rawArray);
};



#endif /* BIG_INT_H */
