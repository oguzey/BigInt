#pragma once
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
	BigInt(std::string &strHexNumber);
	BigInt(BigInt&& number);
	~BigInt();
	BigInt& operator=(BigInt&& number);
	void add(BigInt &number);
	void sub(BigInt &number);
	BigInt* mul(const BigInt &number, BigInt *result);

	///
	///  1 if this > number
	/// -1 if number > this
	///  0 if this == number
	///
	int cmp(BigInt &number);
	int fromString(const char *hexString);
	int fromString(const std::string &hexString);
	std::string toString();
	void shiftLeft(int countBits);
	void shiftRight(int countBits);
	int isEqual(const BigInt &number);
	void setMax();
	void setZero();
	void setNumber(unsigned int number);
	bool isZero();

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
	int getPosMostSignificatnBit();
};

