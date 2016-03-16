#ifndef BIG_INT_H
#define BIG_INT_H
#include <iostream>
#include <string>

static_assert(sizeof(unsigned int) == 4, "Support only 32-bit of integer");


class BigInt {
public:
	BigInt();
	BigInt(unsigned int number);
	BigInt(const char *str_hex_number);
	~BigInt();
	BigInt add(BigInt &a, BigInt &b);
	string to_string();
private:
	unsigned int *array;
	unsigned int length;
	unsigned int size;
}



#endif /* BIG_INT_H */
