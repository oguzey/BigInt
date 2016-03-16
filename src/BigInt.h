#ifndef BIG_INT_H
#define BIG_INT_H
#include <iostream>
#include <string>
#include <string.h>
#include <array>

static_assert(sizeof(unsigned int) == 4, "Support only 32-bit of integer");


class BigInt {
public:
	BigInt();
//	BigInt(unsigned int number);
	BigInt(const char *str_hex_number);
	~BigInt();
	BigInt* add(BigInt &a, BigInt &b);
	int from_string(const char *hex_string);
	int from_string(const std::string &hex_string);
	std::string* to_string();

private:
	unsigned int *blocks;
	unsigned int length;
	unsigned int size;

	int hex_char_to_int(char digit);
	void raw_array_to_blocks(std::array<unsigned int, 32> &raw_array);
};



#endif /* BIG_INT_H */
