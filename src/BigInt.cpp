#include "BigInt.h"
#include <cassert>

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
		std::cout << "Can not convert from string. Check your string." << std::endl;
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
		std::cout << "String too long!" << std::endl;
	}

	for (i = (int)strlen(hex_str) - 1; i >= 0; --i) {
		if (hex_char_to_int(hex_str[i]) == -1) {
			std::cout << "Provided bad nuber (" << hex_str <<")!" << std::endl;
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

	std::cout << "Raw array" << std::endl;
	for(i = 0; i < raw_array.size(); ++i) {
		std::cout << std::dec << i << ")\t" << std::hex << raw_array[i] << std::endl;
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
	std::cout << "BigInt number: " << std::endl;
	for(int i = 0; i < size; ++i) {
		std::cout << std::dec << i << ")\t" << std::hex << blocks[i] << std::endl;
	}
	std::cout << "End BigInt" << std::endl;
	return new std::string("Test string");
}

/******************************************************************************
 * 			Private functions
 *****************************************************************************/

/**
 * @brief 	Convert char in hex format to number.
 * @param digit		- INPUT. Char to convert.
 * @return 		integer in range [0 - 15] when successful and
 * 			-1 if error occurred.
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
 *
 */
void BigInt::raw_array_to_blocks(std::array<block, BIGINT_BYTES> &raw_array)
{
	int left_bits = 0;
	block temp = 0;

	unsigned int index_raw = 0;
	unsigned int index_blocks = 0;

	for(index_raw = 0; index_raw < BIGINT_BYTES; ++index_raw, ++index_blocks) {
		temp = 0;
		if (left_bits) {
			temp = raw_array[index_raw - 1] >> (WORD_BITS - left_bits);
			if (left_bits == BLOCK_BITS) {
				blocks[index_blocks] = temp;
				--index_raw;
				left_bits = 0;
				std::cout << "30 bits| temp = "<< std::hex << temp << std::dec << std::endl;
				continue;
			}
		}
		blocks[index_blocks] = (raw_array[index_raw] << left_bits + BLOCK_CARRY_BITS) >> BLOCK_CARRY_BITS;
		std::cout << index_blocks << "T: " << std::hex << blocks[index_blocks] << " and " << temp << std::endl <<
				std::dec;
		blocks[index_blocks] += temp;
		left_bits += BLOCK_CARRY_BITS;
	}

	assert(left_bits == LAST_BLOCK_BITS);
	assert(index_blocks == BLOCKS_COUNT - 1);

	blocks[index_blocks] = raw_array[index_raw - 1] >> (WORD_BITS - left_bits);
}
