#ifndef ESRABIN_H
#define ESRABIN_H

#include <openssl/sha.h>
#include "BigInt.h"

typedef unsigned char* (hash_func_t)(const unsigned char *d, size_t n, unsigned char *md);

class ESRabinSignature {
public:
	std::string message;
	BigInt R;
	BigInt B;
};

class ESRabinPublicKey {
public:
	BigInt n;
	const string nameHashFunc;
	hash_func_t *hash;
};

class ESRabinPrivateKey {
public:
	BigInt p;
	BigInt q;
};

class ESRabin {
public:
	RandomGenerator gen;

	void generateKeys(ESRabinPublicKey &pubKey, ESRabinPrivateKey &privKey);
	void signMessage(std::string message, ESRabinSignature &signature,
			 const ESRabinPublicKey &pubKey,
			 constESRabinPrivateKey &privKey);
	bool checkSignature(ESRabinSignature &signature);
};

#endif // ESRABIN_H

