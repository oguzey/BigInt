#include <cassert>
#include <algorithm>
#include <math.h>
#include "../logger/logger.h"
#include "../include/BigInt.h"


#define WORD_BITS			32
#define BYTE_BITS			8
#define HEX_CHAR_BITS			4


/* macros for whole BigInt */
#define BIGINT_BITS			1024
#define BIGINT_BYTES			128	/* 1024 / 4 */
#define BIGINT_SIZE_IN_HEX		256

#define BIGINT_DOUBLE_BITS		2048

/*
 * for 1024 bit number need 35 blocks
 * 34 blocks with 30 bits digit
 * and 1 block with 4 bits
 */

/* macros for usual block of BigInt */
#define BLOCK_BITS			30
#define BLOCK_CARRY_BITS		2
#define BLOCK_MAX_NUMBER		0x3FFFFFFF


BigInt::BigInt(unsigned int lengthBits):
	length_(lengthBits),
	size_(ceil(lengthBits / (float)BLOCK_BITS)),
	countBistLastBlock_(lengthBits % BLOCK_BITS),
	maxValueLastBlock_(fillBits(countBistLastBlock_))
{
	assert(lengthBits == BIGINT_BITS || lengthBits == BIGINT_DOUBLE_BITS);

//	INFO("Create BigInt with length {}, size {}, countBistLastBlock {},"
//			"maxValueLastBlock_ {:X}",
//			length_, size_, countBistLastBlock_, maxValueLastBlock_);

	blocks_ = new block[size_];
	memset(blocks_, 0, sizeof(block) * size_);
}

BigInt::BigInt() : BigInt(BIGINT_BITS)
{
}

BigInt::BigInt(const char *strHexNumber) : BigInt()
{
	if (fromString(strHexNumber)) {
		WARN("Can not convert from string. Check your string.");
	}
}

BigInt::BigInt(std::string &strHexNumber) : BigInt()
{
	if (fromString(strHexNumber)) {
		WARN("Can not convert from string. Check your string.");
	}
}

BigInt::BigInt(BigInt&& number) : length_(number.length_), size_(number.size_),
	countBistLastBlock_(number.countBistLastBlock_),
	maxValueLastBlock_(number.countBistLastBlock_)
{
	DEBUG("Move constructor called");

	blocks_ = number.blocks_;
	number.blocks_ = nullptr;
}

BigInt::~BigInt() { delete[] blocks_; }

unsigned int BigInt::getLength()
{
	return length_;
}

int BigInt::fromString(const char *hexStr)
{
	int i = 0;
	size_t maxAmountChars = length_ / HEX_CHAR_BITS;
	std::vector<block> rawArray(length_ / WORD_BITS);
	std::fill(rawArray.begin(), rawArray.end(), 0);

	if (strlen(hexStr) > maxAmountChars) {
		WARN("String too long! Length is '{}'. Possible length is '{}'",
				strlen(hexStr), maxAmountChars);
	}

	for (i = (int)strlen(hexStr) - 1; i >= 0; --i) {
		if (hexCharToInteger(hexStr[i]) == -1) {
			WARN("Provided bad nuber ({})", hexStr);
			return -1;
		}
	}
	unsigned int indexArray = 0;
	unsigned int shiftPos = 0;
	i = (int)strlen(hexStr) - 1;
	for (; i >= 0 && indexArray < rawArray.size(); --i) {
		int digit = hexCharToInteger(hexStr[i]);
		rawArray[indexArray] += digit << 4 * shiftPos;
		++shiftPos;
		if (shiftPos % 8 == 0) {
			++indexArray;
			shiftPos = 0;
		}
	}
	//DEBUG("Raw array");
	//for(i = 0; i < rawArray.size(); ++i) {
	//	DEBUG("{})\t{:x}", i, rawArray[i]);
	//}
	rawArrayToBlocks(rawArray);

	return 0;
}

int BigInt::fromString(const std::string &hexString)
{
	return fromString(hexString.c_str());
}

std::string BigInt::toString()
{
	std::string output("");
	unsigned int i = 0;
	std::vector<block> rawArray(length_ / WORD_BITS);
	std::fill(rawArray.begin(), rawArray.end(), 0);

	blocksToRawArray(rawArray);

	//INFO("BigInt number (normal array): size is {}", rawArray.size());
	for(i = 0; i < rawArray.size(); ++i) {
		//INFO("{}\t{:X}", i, rawArray[i]);
		for (int j = 0; j < 8; ++j) {
			char ch = rawArray[i] >> (j * 4) & 0xF;
			output.push_back(integerToHexChar(ch));
			//DEBUG("parse {} as {}", (int)ch, integerToHexChar(ch));
		}
	}
	//INFO("End of BigInt ( normal array)");
	std::reverse(output.begin(), output.end());
	return output;
}

/**
 * @brief 			Convert char in hex format to number.
 * @param digit			- INPUT. Char to convert.
 * @return 			integer in range [0 - 15] when successful and
 * 				-1 if error occurred.
 */
int BigInt::hexCharToInteger(char digit)
{
	if (digit >= '0' && digit <= '9') {
		return digit - '0' ;
	} else if (digit >= 'a' && digit <= 'f') {
		return 10 + digit - 'a' ;
	} else if (digit >= 'A' && digit <= 'F') {
		return 10 + digit - 'A' ;
	}
	return -1;
}

char BigInt::integerToHexChar(int symbol)
{
	assert(symbol >= 0x0 && symbol <= 0xF);
	if (symbol >= 0 && symbol <= 9) {
		return '0' + symbol;
	} else {
		return 'A' + symbol - 10;
	}
}

void BigInt::rawArrayToBlocks(std::vector<block> &rawArray)
{
	unsigned int leftBits = 0;
	block temp = 0;

	unsigned int indexRaw = 0;
	unsigned int indexBlocks = 0;

	for(; indexRaw < rawArray.size(); ++indexRaw, ++indexBlocks) {
		temp = 0;
		if (leftBits) {
			temp = rawArray[indexRaw - 1] >> (WORD_BITS - leftBits);
			if (leftBits == BLOCK_BITS) {
				blocks_[indexBlocks] = temp;
				--indexRaw;
				leftBits = 0;
				//DEBUG("30 bits| temp = {:X}", temp);
				continue;
			}
		}
		blocks_[indexBlocks] = (rawArray[indexRaw] <<
				(leftBits + BLOCK_CARRY_BITS));
		blocks_[indexBlocks] >>= BLOCK_CARRY_BITS;
		//DEBUG("T: {:X} and {:X}", blocks_[indexBlocks], temp);
		blocks_[indexBlocks] += temp;
		leftBits += BLOCK_CARRY_BITS;
	}

	assert(leftBits == countBistLastBlock_);
	assert(indexBlocks == size_ - 1);

	blocks_[indexBlocks] = rawArray[indexRaw - 1] >> (WORD_BITS - leftBits);
}

/**
 * @brief 			Convert numbers in block representation to
 * 				human readable format.
 * @param rawArray		[output] Array for numbers.
 */
void BigInt::blocksToRawArray(std::vector<block> &rawArray)
{
	unsigned int acquiredBits = 0;
	int indexBlocks = 0;
	int indexRaw = 0;
	int sizeRawArray = rawArray.size();
	block temp = 0;

	for(; indexRaw < sizeRawArray; ++indexBlocks, ++indexRaw) {
		if(acquiredBits == BLOCK_BITS) {
			++indexBlocks;
			acquiredBits = 0;
		}
		rawArray[indexRaw] = blocks_[indexBlocks] >> acquiredBits;
		temp = blocks_[indexBlocks + 1] <<
			(WORD_BITS - acquiredBits - BLOCK_CARRY_BITS);
		//DEBUG("BtA: {:X} + \t {:X}   {}", rawArray[indexRaw], temp,
		//		(WORD_BITS - acquiredBits));
		rawArray[indexRaw] += temp;
		acquiredBits += BLOCK_CARRY_BITS;
	}
}

int BigInt::getPosMostSignificatnBit() const
{
	unsigned int position = length_;
	block temp = 0;
	int found = 0;

	if (isZero()) {
		return -1;
	}

	temp = 1 << (countBistLastBlock_ - 1);
	while (temp && (found = blocks_[size_ - 1] & temp) == 0) {
		--position;
		temp >>= 1;
	}
	if (found) {
		return --position;
	}
	for (int i = size_ - 2; i >= 0; --i) {
		temp = 1 << (BLOCK_BITS - 1);
		while (temp && (found = blocks_[i] & temp) == 0) {
			--position;
			temp >>= 1;
		}
	}
	return --position;
}

int BigInt::isEqual(const BigInt &number)
{
	block *end, *start;

	if (size_ == number.size_) {
		return std::equal(blocks_, blocks_ + size_, number.blocks_);
	} else if (size_ > number.size_) {
		start = blocks_ + number.size_;
		end = blocks_ + size_;
		while (start != end) {
			if (*start++ != 0) {
				return false;
			}
		}
		return std::equal(blocks_, blocks_ + number.size_, number.blocks_);
	} else {
		start = number.blocks_ + size_;
		end = number.blocks_ + number.size_;
		while (start != end) {

			if (*start++ != 0) {
				return false;
			}
		}
		return std::equal(blocks_, blocks_ + size_, number.blocks_);
	}
}

bool BigInt::isZero() const
{
	//block toCmp[size_] = {0};
	//return !memcmp(blocks_, toCmp, size_ * sizeof(block));
	block *start = blocks_;
	block *end = blocks_ + size_;
	while (start != end) {
		if (*start) {
			return false;
		}
		++start;
	}
	return true;
}

void BigInt::setMax()
{
	std::fill(blocks_, blocks_ + size_ - 1, BLOCK_MAX_NUMBER);
	blocks_[size_ - 1] = maxValueLastBlock_;
}

void BigInt::setZero()
{
	memset(blocks_, 0, size_ * sizeof(block));
}

void BigInt::setNumber(unsigned int number)
{
	memset(blocks_ + 1, 0, (size_ - 1) * sizeof(block));
	blocks_[0] = number;
}

block BigInt::fillBits(unsigned int amountBits)
{
	block number = 0;

	for (unsigned int i = 0; i < amountBits; ++i) {
		number += (1 << i);
	}
	return number;
}

void BigInt::shiftLeftBlock(unsigned int countBits)
{
	assert(countBits <= BLOCK_BITS);

	block carryBits = 0;
	block temp = 0;
	unsigned int i = 0;

	for (i = 0; i < size_ - 1; ++i) {
		// acquire carry bits from current block
		temp = blocks_[i] >> (BLOCK_BITS - countBits);
		blocks_[i] = (blocks_[i] << countBits) & BLOCK_MAX_NUMBER;
		// add carry bits from previuos block
		blocks_[i] += carryBits;
		assert(blocks_[i] <= BLOCK_MAX_NUMBER);
		carryBits = temp;
	}
	// last block
	blocks_[i] = (blocks_[i] << countBits) + carryBits;
	blocks_[i] &= maxValueLastBlock_;
}

void BigInt::shiftLeft(unsigned int countBits)
{
	if (countBits >= length_) {
		setZero();
		return;
	}

	while (countBits > BLOCK_BITS) {
		shiftLeftBlock(BLOCK_BITS);
		countBits -= BLOCK_BITS;
	}
	shiftLeftBlock(countBits);
}

void BigInt::shiftRightBlock(unsigned int countBits)
{
	assert(countBits <= BLOCK_BITS);
	unsigned int i = 0;
	block maxCarryBlock = fillBits(countBits);
	block carryBits = 0;

	blocks_[0] >>= countBits;
	for (i = 1; i < size_; ++i) {
		carryBits = blocks_[i] & maxCarryBlock;
		blocks_[i - 1] += carryBits << (BLOCK_BITS - countBits);
		blocks_[i] >>= countBits;
	}
}

void BigInt::shiftRight(unsigned int countBits)
{
	if (countBits >= length_) {
		setZero();
		return;
	}
	while (countBits > BLOCK_BITS) {
		shiftRightBlock(BLOCK_BITS);
		countBits -= BLOCK_BITS;
	}
	shiftRightBlock(countBits);
}

///
///  1 if this > number
/// -1 if number > this
///  0 if this == number
///
int BigInt::cmp(const BigInt &number) const
{
	unsigned int i = 0;
	unsigned int size = size_;
	int diff = 0;
	int j = 0;

	if (size_ > number.size_) {
		for (i = size_ - 1; i > number.size_ - 1; --i) {
			if (blocks_[i]) {
				return 1;
			}
		}
		size = number.size_;
	} else if (number.size_ > size_) {
		for (i = number.size_ - 1; i > size_ - 1; --i) {
			if (number.blocks_[i]) {
				return -1;
			}
		}
		size = size_;
	}

	diff = blocks_[size - 1] - number.blocks_[size - 1];
	for (j = size - 2; j >= 0 && diff == 0; --j) {
		diff = blocks_[j] - number.blocks_[j];
	}
	return diff > 0 ? 1 : diff < 0 ? -1 : 0;
}

void BigInt::setBit(unsigned int position, unsigned int value) const
{
	assert((value & 1) == value);
	assert(position >= 0 && position < length_);

	int posInBlock = position % BLOCK_BITS;
	int posBlock = position / BLOCK_BITS;

	block temp = (~(1 << posInBlock)) & BLOCK_MAX_NUMBER;
	// reset needed bit
	blocks_[posBlock] &= temp;
	// set new value
	blocks_[posBlock] |= (block)value << posInBlock;
	assert(blocks_[size_ - 1] <= maxValueLastBlock_);
}

int BigInt::getBit(unsigned int position) const
{
	assert(position >= 0 && position <= length_);

	block neededBlock = blocks_[position / BLOCK_BITS];
	int posInBlock = position % BLOCK_BITS;
	return (neededBlock & ( 1 << posInBlock )) >> posInBlock;
}

//BigInt &BigInt::copy() const
//{
//	BigInt number(length_);
//	unsigned int i;

//	for (i = 0; i < size_; ++i) {
//		number.blocks_[i] = blocks_[i];
//	}
//	return number;
//}

BigInt* BigInt::copy() const
{
	BigInt *number = new BigInt(length_);
	unsigned int i;

	for (i = 0; i < size_; ++i) {
		number->blocks_[i] = blocks_[i];
	}
	return number;
}

bool BigInt::add(const BigInt &number)
{
	assert(size_ == number.size_);

	block carry = 0;
	unsigned int i = 0;

	for(i = 0 ; i < size_ - 1; ++i) {
		//DEBUG("one {:x} + {:x}", blocks_[i], number.blocks_[i]);
		blocks_[i] = blocks_[i] + number.blocks_[i] + carry;
		carry = blocks_[i] >> BLOCK_BITS;
		blocks_[i] &= BLOCK_MAX_NUMBER;
		//DEBUG("carry = {:x}", carry);
		assert((carry & 1) == carry);
	}

	assert(size_ - 1 == i);

	blocks_[i] = blocks_[i] + number.blocks_[i] + carry;
	if (blocks_[i] > maxValueLastBlock_) {
		DEBUG("carry = {:x}", carry);
		carry = 1;
		assert(1 == 0);
	}
	carry = blocks_[i] >> countBistLastBlock_;
	blocks_[i] &= maxValueLastBlock_;
	return carry;
}

void BigInt::sub(const BigInt &number)
{
	block setterCarryBit = BLOCK_MAX_NUMBER + (block)1;
	block carryBit = 0;
	unsigned int i = 0;

	assert(setterCarryBit == ((block)1 << BLOCK_BITS));

	for (i = 0; i < size_ - 1; ++i) {
		blocks_[i] |= setterCarryBit;
		blocks_[i] -= number.blocks_[i] + carryBit;
		carryBit = !(blocks_[i] >> BLOCK_BITS);
		blocks_[i] &= BLOCK_MAX_NUMBER;
		assert((carryBit & 1) == carryBit);
	}
	// last block
	blocks_[i] = blocks_[i] - number.blocks_[i] - carryBit;
	blocks_[i] &= maxValueLastBlock_;
}

BigInt* BigInt::mul(const BigInt &number, BigInt **result)
{
	uint64_t temp_res = 0;
	block c = 0;
	BigInt *res = NULL;
	if (result) {
		if (*result) {
			res = *result;
			assert(res->length_ == BIGINT_DOUBLE_BITS);
			res->setZero();
		} else {
			res = *result = new BigInt(BIGINT_DOUBLE_BITS);
		}
	} else {
		res = new BigInt(BIGINT_DOUBLE_BITS);
	}

	assert(res != NULL);
	assert(res->length_ == BIGINT_DOUBLE_BITS);

	for(unsigned int i = 0; i < size_; ++i) {
		c = 0;
		for (unsigned int j = 0; j < number.size_; ++j) {
			temp_res = res->blocks_[i + j] + blocks_[i] * number.blocks_[j] + c;
			res->blocks_[i + j] = temp_res & BLOCK_MAX_NUMBER;
			c = (temp_res >> BLOCK_BITS) & BLOCK_MAX_NUMBER;
		}
		res->blocks_[i + number.size_] = c;
	}
	return res;
}

void BigInt::mulByBit(int bitValue)
{
	assert((bitValue & 1) == bitValue);

	if (bitValue == 0) {
		setZero();
	}
}

bool BigInt::div(const BigInt &N, const BigInt &D, BigInt *Q, BigInt *R)
{
	int res;

	if (D.isZero()) {
		WARN("Could not divide by zero.");
		return false;
	}

	Q = new BigInt(BIGINT_BITS);
	R = new BigInt(BIGINT_BITS);

	int i;
	for (i = N.length_ - 1; i >= 0; --i) {
		R->shiftLeftBlock(1);
		R->setBit(0, N.getBit(i));
		res = R->cmp(D);
		if (res == 1 || res == 0) {
			R->sub(D);
			Q->setBit(i, 1);
		}
	}
	return true;
}

BigInt* BigInt::montMul(const BigInt &y, const BigInt &m)
{
	assert(length_ == y.length_);
	assert(length_ == m.length_);

	assert(cmp(m) == -1);
	assert(y.cmp(m) == -1);

	// gcd(m; b) = 1
	// b == 2
	// m should be odd
	assert(m.getBit(0) == 1);

	// this - x
	BigInt *A = new BigInt();

	bool overflow = false;
	unsigned int u = 0;
	unsigned int y0 = y.getBit(0);
	unsigned int xi = 0;
	unsigned int i;

	// fing max len of numbers
	unsigned int len = m.getPosMostSignificatnBit();

	DEBUG("max len is {}", len);

	for (i = 0; i < length_; ++i) {
		overflow = false;

		xi = this->getBit(i);
		assert((xi & 1) == xi);
		assert((A->getBit(0) & 1) == A->getBit(0));
		//DEBUG("{})xi = {:x}", i, xi);
		u = (A->getBit(0) + xi * y0) % 2;
		assert((u & 1) == u);

		if (xi) {
			overflow = A->add(y);
			//DEBUG("Overflow A + y = {}", overflow);
		}
		if (u) {
			bool overflow2 = A->add(m);
			//DEBUG("Overflow A + m = {}", overflow2);
			if (overflow) {
				if (overflow2) {
					assert(1 == 0);
				} else {
					assert(2 == 0);
				}
			}
			overflow = overflow2;
		}
		assert(A->getBit(0) == 0);
		A->shiftRightBlock(1);
		if (overflow) {
			//DEBUG("Set overflowed bit");
			A->setBit(length_ - 1, 1);
		}
		//DEBUG("temp {}", A->toString());
	}
	if (A->cmp(m) == 1) {
		A->sub(m);
		//DEBUG("SUB");
	}
	return A;
}

BigInt* BigInt::mod(const BigInt &m)
{
	BigInt *r = NULL;
	BigInt *tmp = NULL;
	int q = 0;

	if (cmp(m) == -1) {
		return this;
	}
	r = new BigInt(BIGINT_DOUBLE_BITS);
	q = 1;
}

