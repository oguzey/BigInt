#include "BigInt.h"
#include <assert>
#include <string.h>

/* macros for whole BigInt */
#define BIGINT_BITS		1024
#define BIGINT_SIZE_IN_HEX	256

/* macros for usual block of BigInt */
#define BLOCK_BITS		30
#define BLOCK_MAX_NUMBER	0x3FFFFFFF
#define BLOCK_SIZE_IN_HEX	4

/* macros for last block of BigInt */
#define LAST_BLOCK_BITS		4
#define LAST_BLOCK_MAX_NUMBER	0xF

typedef block unsigned int;

BigInt::BigInt()
{
	/*
	 * for 1024 bit number need 35 blocks
	 * 34 blocks with 30 bits digit
	 * and 1 block with 4 bits
	 */
	size = 35;
	array = new block[size];
	length = 0;
}

BigInt::BigInt(const char *str_hex_number)
{
	char number_str[BIGINT_SIZE_IN_HEX + 1] = {0};
	char block_str[BLOCK_SIZE_IN_HEX + 1] = {0};

	for( int i = strlen(str_hex_number) - 1; i >= 0 ; --i )
	{
		//TODO: implement me
	}
	


	BigInt *number = new BigInt();
	unsigned int length = strlen(str_hex_number);
	unsigned int i = 0;
	unsigned int capable_string_size = BIGINT_BITS / 4;
	unsigned int start_pos = length - 1;

	if( length > capable_string_size )
	{
		// TODO: Write log about overflow
		start_pos = capable_string_size - 1;
	}

	

	return number;
}

BigInt* BigInt::add(BigInt &a, BigInt &b)
{
	BigInt *summ = new BigInt();
	block carry = 0;

	for( int i = 0 ; i < size - 1; ++i )
	{
		summ[i] = a[i] + b[i] + carry;
		carry = summ[i] >> BLOCK_BITS;
		summ[i] &= BLOCK_MAX_NUMBER;
		assert(carry & 1 == carry);
	}

	assert(34 == i);

	summ[i] = a[i] + b[i] + carry;
	carry = summ[i] >> LAST_BLOCK_BITS;
	summ[i] &= LAST_BLOCK_MAX_NUMBER;

	return summ;
}

