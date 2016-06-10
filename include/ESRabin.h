#ifndef ESRABIN_H
#define ESRABIN_H

#include "BigInt.h"

typedef unsigned char* (hash_func_t)(const unsigned char *d, size_t n, unsigned char *md);

class ESRabinSignature {
	friend class ESRabinManager;
public:
	const std::string& getMessage() const { return message; }
	const BigInt& getR() const { return R; }
	const BigInt& getB() const { return B; }
private:
	std::string message;
	BigInt R;
	BigInt B;
};

class ESRabinPublicKey {
	friend class ESRabinManager;
public:
	const BigInt& getN() const { return n; }
	const std::string& getHash() const { return nameHashFunc; }
private:
	BigInt n;
	std::string nameHashFunc;
	hash_func_t *hash;
};

class ESRabinPrivateKey {
	friend class ESRabinManager;
public:
	const BigInt& getP() const { return p; }
	const BigInt& getQ() const { return q; }
private:
	BigInt p;
	BigInt q;
};

class ESRabinManager {
public:
	ESRabinManager(RandomGenerator &gen): generator(gen) {}
	void generateKeys(ESRabinPublicKey &pubKey, ESRabinPrivateKey &privKey);
	void finalizeKeys(ESRabinPublicKey &pubKey, ESRabinPrivateKey &privKey);
	void signMessage(const std::string &message, ESRabinSignature &signature,
			 const ESRabinPublicKey &pubKey,
			 const ESRabinPrivateKey &privKey);
	bool checkSignature(const ESRabinSignature &signature,
			    const ESRabinPublicKey &pubKey);
private:
	RandomGenerator& generator;
	void calculateBeta(ESRabinSignature &signature,
			   const ESRabinPublicKey &pubKey,
			   const ESRabinPrivateKey &privKey,
			   const BigInt &H);

	void GarnerAlgorithmCRT(const BigInt &p, const BigInt &q,
				const BigInt &Vp, const BigInt &Vq, BigInt &res);
};

#endif // ESRABIN_H

