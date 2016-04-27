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
	///
	/// only for tests
	///
	static BigInt* getDoubleNumber();
	unsigned int getLength();
	bool add(const BigInt &number);
	void sub(const BigInt &number);
	void mulByBit(int bitValue);
	bool div(const BigInt &N, const BigInt &D, BigInt *Q, BigInt *R);
	void mulMont(const BigInt &y, const BigInt &m, BigInt &ret) const;
	void mod(const BigInt &m);
	void exp(const BigInt &e, const BigInt &m, BigInt &ret);

	///
	///  1 if this > number
	/// -1 if this < number
	///  0 if this == number
	///
	int cmp(const BigInt &number) const;
	int fromString(const char *hexString);
	int fromString(const std::string &hexString);
	std::string toString() const;
	void shiftLeftBlock(unsigned int countBits);
	void shiftLeft(unsigned int countBits);
	void shiftRightBlock(unsigned int countBits);
	void shiftRightBit();
	void shiftRight(unsigned int countBits);
	void setBit(unsigned int position, unsigned int value) const;
	int getBit(unsigned int position) const;
	int clearBit(unsigned int position);
	BigInt* copy() const;
	void copyContent(const BigInt &number);
	int isEqual(const BigInt &number);
	void setMax();
	void setZero();
	void setNumber(unsigned int number);
	bool isZero() const;
	int getPosMostSignificatnBit() const;
	///
	/// Create pre-compilation table for modular reduction.
	/// Should be called for module m.
	/// Need for methods modular reduction `mod` and Montgomery
	/// multiplication `mulMont`.
	///
	void initModularReduction();
	///
	/// Destroy pre-compilation table for modular reduction.
	/// Should be called for module m.
	/// Need for methods modular reduction `mod` and Montgomery
	/// multiplication `mulMont`.
	///
	void shutDownModularReduction();

private:
	block *blocks_;
	const unsigned int length_;
	const unsigned int size_;
	const unsigned int countBistLastBlock_;
	const block maxValueLastBlock_;

	///
	/// Using only for Montgomery multiplication and modular reduction.
	///
	BigInt **preComputedTable_;
	int posMostSignBit_;

	BigInt(unsigned int lengthBits);
	int hexCharToInteger(char digit);
	void rawArrayToBlocks(std::vector<block> &rawArray);
	void blocksToRawArray(std::vector<block> &rawArray) const;
	char integerToHexChar(int symbol) const;
	void splitToRWords(std::vector<block> &rWords, int lenBits) const;
	static block fillBits(unsigned int amountBits);
};

