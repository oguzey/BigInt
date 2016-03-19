#include "../include/BigInt.h"
#include <cassert>
#include "../logger/logger.h"
#include <algorithm>

/* macros for whole BigInt */
#define BIGINT_BITS				1024
#define BIGINT_BYTES			32
#define BIGINT_SIZE_IN_HEX		256

#define WORD_BITS				32
/*
 * for 1024 bit number need 35 blocks
 * 34 blocks with 30 bits digit
 * and 1 block with 4 bits
 */

#define BLOCKS_COUNT			35
#define BLOCK_CARRY_BITS		2

/* macros for usual block of BigInt */
#define BLOCK_BITS				30
#define BLOCK_MAX_NUMBER		0x3FFFFFFF
#define BLOCK_SIZE_IN_HEX		4

/* macros for last block of BigInt */
#define LAST_BLOCK_BITS			4
#define LAST_BLOCK_MAX_NUMBER	0xF

typedef  unsigned int block;

BigInt::BigInt()
{
	size_ = BLOCKS_COUNT;
	blocks_ = new block[size_];
	memset(blocks_, 0, sizeof(block) * size_);
	length_ = 0;
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
	int i = 0;

	for(i = 0 ; i < size_ - 1; ++i) {
		DEBUG("one {:x} + {:x}", blocks_[i], number.blocks_[i]);
		blocks_[i] = blocks_[i] + number.blocks_[i] + carry;
		carry = blocks_[i] >> BLOCK_BITS;
		blocks_[i] &= BLOCK_MAX_NUMBER;
		DEBUG("carry = {:x}", carry);
		assert((carry & 1) == carry);
	}

	assert(BLOCKS_COUNT - 1 == i);

	blocks_[i] = blocks_[i] + number.blocks_[i] + carry;
	carry = blocks_[i] >> LAST_BLOCK_BITS;
	blocks_[i] &= LAST_BLOCK_MAX_NUMBER;
}

int BigInt::fromString(const char *hexStr)
{
	int i = 0;
	std::array<block, BIGINT_BYTES> rawArray = {0};

	if (strlen(hexStr) > BIGINT_SIZE_IN_HEX) {
		WARN("String too long!");
	}

	for (i = (int)strlen(hexStr) - 1; i >= 0; --i) {
		if (hexCharToInteger(hexStr[i]) == -1) {
			WARN("Provided bad nuber ({})", hexStr);
			return -1;
		}
	}
	int indexArray = 0;
	int shiftPos = 0;
	for (i = (int)strlen(hexStr) - 1; i >= 0 && indexArray < size_; --i) {
		int digit = hexCharToInteger(hexStr[i]);
		rawArray[indexArray] += digit << 4 * shiftPos;
		++shiftPos;
		if (shiftPos % 8 == 0) {
			++indexArray;
			shiftPos = 0;
		}
	}
	DEBUG("Raw array");
	for(i = 0; i < rawArray.size(); ++i) {
		DEBUG("{})\t{:x}", i, rawArray[i]);
	}
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
	int i = 0;
	std::array<block, BIGINT_BYTES> rawArray = {0};

	INFO("BigInt number (blocks): ");
	for(; i < size_; ++i) {
		INFO("{}\t{:X}", i, blocks_[i]);
	}
	INFO("End BigInt (blocks)");

	blocksToRawArray(rawArray);

	INFO("BigInt number (normal array): ");
	for(i = 0; i < rawArray.size(); ++i) {
		INFO("{}\t{:X}", i, rawArray[i]);
		for (int j = 0; j < 8; ++j) {
			char ch = rawArray[i] >> (j * 4) & 0xF;
			output->push_back(integerToHexChar(ch));
			DEBUG("parse {} as {}", (int)ch, integerToHexChar(ch));
		}
	}
	INFO("End of BigInt ( normal array)");
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
	char out = -1;
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
void BigInt::rawArrayToBlocks(std::array<block, BIGINT_BYTES> &rawArray)
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
				DEBUG("30 bits| temp = {:X}", temp);
				continue;
			}
		}
		blocks_[indexBlocks] = (rawArray[indexRaw] <<
				leftBits + BLOCK_CARRY_BITS) >> BLOCK_CARRY_BITS;
		DEBUG("T: {:X} and {:X}", blocks_[indexBlocks], temp);
		blocks_[indexBlocks] += temp;
		leftBits += BLOCK_CARRY_BITS;
	}

	assert(leftBits == LAST_BLOCK_BITS);
	assert(indexBlocks == BLOCKS_COUNT - 1);

	blocks_[indexBlocks] = rawArray[indexRaw - 1] >> (WORD_BITS - leftBits);
}

/**
 * @brief 			Convert numbers in block representation to
 * 				human readable format.
 * @param rawArray		[output] Array for numbers.
 */
void BigInt::blocksToRawArray(std::array<block, BIGINT_BYTES> &rawArray)
{
	unsigned int acquiredBits = 0;
	int indexBlocks = 0,
		indexRaw = 0;
	block temp = 0;

	for(; indexRaw < BIGINT_BYTES; ++indexBlocks, ++indexRaw) {
		if(acquiredBits == BLOCK_BITS) {
			++indexBlocks;
			acquiredBits = 0;
		}
		rawArray[indexRaw] = blocks_[indexBlocks] >> acquiredBits;
		temp = blocks_[indexBlocks + 1] <<
			(WORD_BITS - acquiredBits - BLOCK_CARRY_BITS);
		DEBUG("BtA: {:X} + \t {:X}   {}", rawArray[indexRaw], temp,
				(WORD_BITS - acquiredBits));
		rawArray[indexRaw] += temp;
		acquiredBits += 2;
	}
}

