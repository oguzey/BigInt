#include <cassert>
#include <algorithm>
#include <math.h>
#include "../logger/logger.h"
#include "../include/BigInt.h"


/* macros for whole BigInt */
#define BIGINT_BITS			1024
#define BIGINT_BYTES			32
#define BIGINT_SIZE_IN_HEX		256

#define BIGINT_DOUBLE_BITS		2048


#define WORD_BITS			32
#define BYTE_BITS			8
/*
 * for 1024 bit number need 35 blocks
 * 34 blocks with 30 bits digit
 * and 1 block with 4 bits
 */

#define BLOCKS_COUNT			35
#define BLOCKS_DOUBLE_COUNT		69
#define BLOCK_CARRY_BITS		2

/* macros for usual block of BigInt */
#define BLOCK_BITS			30
#define BLOCK_MAX_NUMBER		0x3FFFFFFF
#define BLOCK_SIZE_IN_HEX		4


BigInt::BigInt(unsigned int lengthBits):
	length_(lengthBits),
	size_(ceil(lengthBits / (float)BLOCK_BITS)),
	countBistLastBlock_(lengthBits % BLOCK_BITS),
	maxValueLastBlock_(fillBits(countBistLastBlock_))
{
	assert(lengthBits == BIGINT_BITS || lengthBits == BIGINT_DOUBLE_BITS);

	INFO("Create BigInt with length {}, size {}, countBistLastBlock {},"
			"maxValueLastBlock_ {:X}",
			length_, size_, countBistLastBlock_, maxValueLastBlock_);

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

BigInt::~BigInt() { delete[] blocks_; }

void BigInt::add(BigInt &number)
{
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

	assert(BLOCKS_COUNT - 1 == i);

	blocks_[i] = blocks_[i] + number.blocks_[i] + carry;
	carry = blocks_[i] >> countBistLastBlock_;
	blocks_[i] &= maxValueLastBlock_;
}

void BigInt::sub(BigInt &number)
{
	block setterCarryBit = BLOCK_MAX_NUMBER + 1;
	block carryBit = 0;
	unsigned int i = 0;

	assert(setterCarryBit == (1 << (BLOCK_BITS + 1)));

	for (i = 0; i < size_ - 1; ++i) {
		blocks_[i] = (blocks_[i] | setterCarryBit) - number.blocks_[i] - carryBit;
		carryBit = blocks_[i] >> BLOCK_BITS;
		assert((carryBit & 1) == carryBit);
		carryBit ^= 1;
	}
	// last block
	blocks_[i] = blocks_[i] - number.blocks_[i] - carryBit;
	blocks_[i] &= maxValueLastBlock_;
}

int BigInt::fromString(const char *hexStr)
{
	int i = 0;
	size_t maxAmountChars = length_ / BYTE_BITS;
	std::vector<block> rawArray(length_ / WORD_BITS);
	std::fill(rawArray.begin(), rawArray.end(), 0);

	if (strlen(hexStr) > maxAmountChars) {
		WARN("String too long!");
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

std::string* BigInt::toString()
{
	std::string *output = new std::string("");
	unsigned int i = 0;
	std::vector<block> rawArray(length_ / WORD_BITS);
	std::fill(rawArray.begin(), rawArray.end(), 0);

	//INFO("BigInt number (blocks): ");
	//for(; i < size_; ++i) {
	//	INFO("{}\t{:X}", i, blocks_[i]);
	//}
	//INFO("End BigInt (blocks)");

	blocksToRawArray(rawArray);

	INFO("BigInt number (normal array): size is {}", rawArray.size());
	for(i = 0; i < rawArray.size(); ++i) {
		//INFO("{}\t{:X}", i, rawArray[i]);
		for (int j = 0; j < 8; ++j) {
			char ch = rawArray[i] >> (j * 4) & 0xF;
			output->push_back(integerToHexChar(ch));
			//DEBUG("parse {} as {}", (int)ch, integerToHexChar(ch));
		}
	}
	//INFO("End of BigInt ( normal array)");
	std::reverse(output->begin(), output->end());
	return output;
}

/******************************************************************************
 * 			Private functions
 *****************************************************************************/

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
/**
 * @brief rawArray_to_blocks	Convert array with numbers to array in block`s representaion.
 * @param rawArray		[input] Array with numbers.
 */
void BigInt::rawArrayToBlocks(std::vector<block> &rawArray)
{
	int leftBits = 0;
	block temp = 0;

	unsigned int indexRaw = 0;
	unsigned int indexBlocks = 0;

	for(; indexRaw < BIGINT_BYTES; ++indexRaw, ++indexBlocks) {
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
	assert(indexBlocks == BLOCKS_COUNT - 1);

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
		acquiredBits += 2;
	}
}

block BigInt::fillBits(unsigned int amountBits)
{
	block number = 0;

	for (unsigned int i = 0; i < amountBits; ++i) {
		number += (1 << i);
	}
	return number;
}

void BigInt::shiftLeft(int countBits)
{
	assert(countBits >=0 && countBits <= BLOCK_BITS);

	block carryBits = 0;
	block temp = 0;
	unsigned int i = 0;

	for (i = 0; i < size_ - 1; ++i) {
		// acquire carry bits from current block
		temp = blocks_[i] >> (BLOCK_BITS - countBits);
		blocks_[i] = (blocks_[i] << countBits) & BLOCK_MAX_NUMBER;
		// add carry bits from previuos block
		blocks_[i] += carryBits;
		carryBits = temp;
	}
	// last block
	blocks_[i] = (blocks_[i] << countBits) + carryBits;
	blocks_[i] &= maxValueLastBlock_;
}

void BigInt::shiftRight(int countBits)
{
	assert(countBits >= 0 && countBits <= BLOCK_BITS);
	unsigned int i = 0;
	block maxCarryBlock = fillBits(countBits);
	block carryBits = 0;

	//for (i = 0; i < countBits; ++i) {
	//	maxCarryBlock += (1 << i);
	//}
	DEBUG("maxCarryBlock = {:X}", maxCarryBlock);

	blocks_[0] >>= countBits;
	for (i = 1; i < size_; ++i) {
		carryBits = blocks_[i] & maxCarryBlock;
		blocks_[i - 1] += carryBits << (BLOCK_BITS - countBits);
		blocks_[i] >>= countBits;
	}
}


BigInt* BigInt::mul(const BigInt &number, BigInt *result)
{
	uint64_t temp_res = 0;
	block c = 0;
	BigInt *res = result ? result : new BigInt(BIGINT_DOUBLE_BITS);

	assert(res->size_ == BIGINT_DOUBLE_BITS);

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

int BigInt::getPosMostSignificatnBit()
{
	unsigned int position = 0;
	block temp = 0;
	int found = 0;
	temp = 1 << countBistLastBlock_;
	while (temp && (found = blocks_[size_ - 1] & temp) == 0) {
		++position;
		temp >>= 1;
	}
	if (found) {
		return position;
	}
	for (int i = size_ - 2; i >= 0; --i) {
		temp = 1 << BLOCK_BITS;
		while (temp && (found = blocks_[i] & temp) == 0) {
			++position;
			temp >>= 1;
		}
	}
	return position ? position : -1;
}

int BigInt::isEqual(const BigInt &number)
{
	assert(size_ == number.size_ == BIGINT_BITS);
	return std::equal(blocks_, blocks_ + size_, number.blocks_);
}

void BigInt::setMax()
{
	memset(blocks_, BLOCK_MAX_NUMBER, size_ - 1);
	blocks_[size_ - 1] = maxValueLastBlock_;
}

void BigInt::setZero()
{
	memset(blocks_, 0, size_);
}

void BigInt::setNumber(unsigned int number)
{
	memset(blocks_ + 1, 0, size_ - 1);
	blocks_[0] = number;
}


