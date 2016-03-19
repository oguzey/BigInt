#include "../include/BigInt.h"
#include <cassert>
#include "../CommonLib/logger.h"

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
	size = BLOCKS_COUNT;
	blocks = new block[size];
	memset(blocks, 0, sizeof(block) * size);
	length = 0;
}

BigInt::BigInt(const char *str_hex_number) : BigInt()
{
	if (from_string(str_hex_number)) {
		WARN("Can not convert from string. Check your string.");
	}
}

BigInt::~BigInt() { delete[] blocks; }

BigInt* BigInt::add(BigInt &a, BigInt &b)
{
	BigInt *summ = new BigInt();
	block carry = 0;
	int i = 0;

	for(i = 0 ; i < size - 1; ++i) {
		summ->blocks[i] = a.blocks[i] + b.blocks[i] + carry;
		carry = summ->blocks[i] >> BLOCK_BITS;
		summ->blocks[i] &= BLOCK_MAX_NUMBER;
		assert(carry & 1 == carry);
	}

	assert(BLOCKS_COUNT - 1 == i);

	summ->blocks[i] = a.blocks[i] + b.blocks[i] + carry;
	carry = summ->blocks[i] >> LAST_BLOCK_BITS;
	summ->blocks[i] &= LAST_BLOCK_MAX_NUMBER;

	return summ;
}

int BigInt::from_string(const char *hex_str)
{
	int i = 0;
	std::array<block, BIGINT_BYTES> raw_array = {0};

	if (strlen(hex_str) > BIGINT_SIZE_IN_HEX) {
		WARN("String too long!");
	}

	for (i = (int)strlen(hex_str) - 1; i >= 0; --i) {
		if (hex_char_to_int(hex_str[i]) == -1) {
			WARN("Provided bad nuber ({})", hex_str);
			return -1;
		}
	}
	int index_array = 0;
	int shift_pos = 0;
	for (i = (int)strlen(hex_str) - 1; i >= 0 && index_array < size; --i) {
		int digit = hex_char_to_int(hex_str[i]);
		raw_array[index_array] += digit << 4 * shift_pos;
		++shift_pos;
		if (shift_pos % 8 == 0) {
			++index_array;
			shift_pos = 0;
		}
	}
	DEBUG("Raw array");
	for(i = 0; i < raw_array.size(); ++i) {
		DEBUG("{})\t{:x}", i, raw_array[i]);
	}
	raw_array_to_blocks(raw_array);

	return 0;
}

int BigInt::from_string(const std::string &hex_string)
{
	return from_string(hex_string.c_str());
}

std::string* BigInt::to_string()
{
	int i = 0;
	std::array<block, BIGINT_BYTES> raw_array = {0};

	INFO("BigInt number (blocks): ");
	for(; i < size; ++i) {
		INFO("{}\t{:X}", i, blocks[i]);
	}
	INFO("End BigInt (blocks)");

	blocks_to_raw_array(raw_array);

	INFO("BigInt number (normal array): ");
	for(i = 0; i < raw_array.size(); ++i) {
		INFO("{}\t{:X}", i, raw_array[i]);
	}
	INFO("End of BigInt ( normal array)");

	return new std::string("Test string");
}

/******************************************************************************
 * 			Private functions
 *****************************************************************************/

/**
 * @brief hex_char_to_int	Convert char in hex format to number.
 * @param digit			- INPUT. Char to convert.
 * @return 			integer in range [0 - 15] when successful and
 * 				-1 if error occurred.
 */
int BigInt::hex_char_to_int(char digit)
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

/**
 * @brief raw_array_to_blocks	Convert array with numbers to array in block`s representaion.
 * @param raw_array		[input] Array with numbers.
 */
void BigInt::raw_array_to_blocks(std::array<block, BIGINT_BYTES> &raw_array)
{
	int left_bits = 0;
	block temp = 0;

	unsigned int index_raw = 0;
	unsigned int index_blocks = 0;

	for(; index_raw < BIGINT_BYTES; ++index_raw, ++index_blocks) {
		temp = 0;
		if (left_bits) {
			temp = raw_array[index_raw - 1] >> (WORD_BITS - left_bits);
			if (left_bits == BLOCK_BITS) {
				blocks[index_blocks] = temp;
				--index_raw;
				left_bits = 0;
				DEBUG("30 bits| temp = {:X}", temp);
				continue;
			}
		}
		blocks[index_blocks] = (raw_array[index_raw] <<
				left_bits + BLOCK_CARRY_BITS) >> BLOCK_CARRY_BITS;
		DEBUG("T: {:X} and {:X}", blocks[index_blocks], temp);
		blocks[index_blocks] += temp;
		left_bits += BLOCK_CARRY_BITS;
	}

	assert(left_bits == LAST_BLOCK_BITS);
	assert(index_blocks == BLOCKS_COUNT - 1);

	blocks[index_blocks] = raw_array[index_raw - 1] >> (WORD_BITS - left_bits);
}

/**
 * @brief blocks_to_raw_array	Convert numbers in block representation to
 * 				human readable format.
 * @param raw_array		[output] Array for numbers.
 */
void BigInt::blocks_to_raw_array(std::array<block, BIGINT_BYTES> &raw_array)
{
	unsigned int acquired_bits = 0;
	int index_blocks = 0,
		index_raw = 0;
	block temp = 0;

	for(; index_raw < BIGINT_BYTES; ++index_blocks, ++index_raw) {
		if(acquired_bits == BLOCK_BITS) {
			++index_blocks;
			acquired_bits = 0;
		}
		raw_array[index_raw] = blocks[index_blocks] >> acquired_bits;
		temp = blocks[index_blocks + 1] <<
			(WORD_BITS - acquired_bits + BLOCK_CARRY_BITS);
		raw_array[index_raw] += temp;
		acquired_bits += 2;
	}
}

