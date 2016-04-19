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
	BigInt& operator=(BigInt&& number) = delete;
	unsigned int getLength();
	bool add(const BigInt &number);
	void sub(const BigInt &number);
	BigInt* mul(const BigInt &number, BigInt **result);
	void mulByBit(int bitValue);
	bool div(const BigInt &N, const BigInt &D, BigInt *Q, BigInt *R);
	BigInt* montMul(const BigInt &y, const BigInt &m);

	///
	///  1 if this > number
	/// -1 if number > this
	///  0 if this == number
	///
	int cmp(const BigInt &number) const;
	int fromString(const char *hexString);
	int fromString(const std::string &hexString);
	std::string toString();
	void shiftLeftBlock(unsigned int countBits);
	void shiftLeft(unsigned int countBits);
	void shiftRightBlock(unsigned int countBits);
	void shiftRight(unsigned int countBits);
	void setBit(unsigned int position, unsigned int value) const;
	int getBit(unsigned int position) const;
	BigInt* copy() const;
	int isEqual(const BigInt &number);
	void setMax();
	void setZero();
	void setNumber(unsigned int number);
	bool isZero() const;
	int getPosMostSignificatnBit() const;
	void* initMontMul() const;
	void shutDownMontMul(void *obj) const;

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

