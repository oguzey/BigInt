#ifndef ESRABIN_H
#define ESRABIN_H

#include "BigInt.h"

typedef unsigned char* (hash_func_t)(const unsigned char *d, size_t n, unsigned char *md);

class ESRabinSignature {
	friend class ESRabinManager;
public:
	const std::string& getMessage() { return message; }
	const BigInt& getR() { return R; }
	const BigInt& getB() { return B; }
private:
	std::string message;
	BigInt R;
	BigInt B;
};

class ESRabinPublicKey {
	friend class ESRabinManager;
public:
	const BigInt& getN() { return n; }
	const std::string& getHash() { return nameHashFunc; }
private:
	BigInt n;
	std::string nameHashFunc;
	hash_func_t *hash;
};

class ESRabinPrivateKey {
	friend class ESRabinManager;
public:
	const BigInt& getP() { return p; }
	const BigInt& getQ() { return q; }
private:
	BigInt p;
	BigInt q;
};

class ESRabinManager {
public:
	void generateKeys(ESRabinPublicKey &pubKey, ESRabinPrivateKey &privKey);
	void signMessage(const std::string &message, ESRabinSignature &signature,
			 const ESRabinPublicKey &pubKey,
			 const ESRabinPrivateKey &privKey);
	bool checkSignature(ESRabinSignature &signature);
private:
	RandomGenerator& gen = RandomGeneratorMush::getGeneratorMush();
};

#endif // ESRABIN_H

