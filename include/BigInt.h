#ifndef BIG_INT_H
#define BIG_INT_H
#include <iostream>
#include <string>
#include <string.h>
#include <array>
#include <vector>

static_assert(sizeof(unsigned int) == 4, "Support only 32-bit of integer");


typedef  unsigned int block;


class BigInt {
public:
	BigInt();
	BigInt(const char *strHexNumber);
	~BigInt();
	void add(BigInt &number);
	int fromString(const char *hexString);
	int fromString(const std::string &hexString);
	std::string* toString();
	void shiftLeft(int countBits);
	void shiftRight(int countBits);
	BigInt* mul(const BigInt &number, BigInt *result);

private:
	block *blocks_;
	const unsigned int length_;
	const unsigned int size_;
	const unsigned int countBistLastBlock_;
	const block maxValueLastBlock_;

	BigInt(unsigned int lengthBits);
	int hexCharToInteger(char digit);
	void rawArrayToBlocks(std::vector<block> &rawArray);
	void blocksToRawArray(std::vector<block> &rawArray);
	char integerToHexChar(int symbol);
	block fillBits(unsigned int amountBits);
};



#endif /* BIG_INT_H */
