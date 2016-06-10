#include <openssl/sha.h>
#include <assert.h>
#include "ESRabin.h"
#include "logger.h"

void ESRabinManager::generateKeys(ESRabinPublicKey &pubKey, ESRabinPrivateKey &privKey)
{
	pubKey.n.generateBlumPrime(generator, privKey.p, privKey.q);
	pubKey.hash = SHA256;
	pubKey.nameHashFunc.assign("SHA256");

	pubKey.n.initModularReduction();
	privKey.p.initModularReduction();
	privKey.q.initModularReduction();
}

void ESRabinManager::finalizeKeys(ESRabinPublicKey &pubKey, ESRabinPrivateKey &privKey)
{
	pubKey.n.shutDownModularReduction();
	privKey.p.shutDownModularReduction();
	privKey.q.shutDownModularReduction();
}

void ESRabinManager::signMessage(const std::string &message, ESRabinSignature &signature,
		 const ESRabinPublicKey &pubKey,
		 const ESRabinPrivateKey &privKey)
{
	std::vector<uint8_t> byteArray;
	std::vector<uint8_t> bytes(message.size());
	unsigned char digest[SHA256_DIGEST_LENGTH] = {0};

	BigInt expP, expQ, one, res, H;

	one.setNumber(1);

	expP.copyContent(privKey.p);
	expP.shiftRightBit(); // do not need sub one

	expQ.copyContent(privKey.q);
	expQ.shiftRightBit(); // do not need sub one

	signature.message.assign(message);

	while (true) {
		signature.R.generateRand(generator);
		signature.R.getByteArray(byteArray);

		bytes.clear();
		bytes.insert(bytes.begin(), signature.message.begin(), signature.message.end());
		bytes.insert(bytes.end(), byteArray.begin(), byteArray.end());

		pubKey.hash(bytes.data(), bytes.size(), digest);
		H.fromByteArray(digest, SHA256_DIGEST_LENGTH);

		H.exp(expP, privKey.p, res);
		if (res.isEqual(one)) {
			H.exp(expQ, privKey.q, res);
			if (res.isEqual(one)) {
				INFO("Current H is qadratic residue. '{}'", H.toString());
				break;
			}
		}
		DEBUG("Number '{}' is not qadratic residue", H.toString());
	}
	// calculate B
	calculateBeta(signature, pubKey, privKey, H);
}

void ESRabinManager::calculateBeta(ESRabinSignature &signature,
				   const ESRabinPublicKey &pubKey,
				   const ESRabinPrivateKey &privKey,
				   const BigInt &H)
{
	BigInt expP, expQ, one, rootForQ, rootForP;

	one.setNumber(1);

	// calculate H^0.5
	expP.copyContent(privKey.p);
	expP.add(one);
	expP.shiftRightBlock(2);

	expQ.copyContent(privKey.q);
	expQ.add(one);
	expQ.shiftRightBlock(2);

	H.exp(expP, privKey.p, rootForP);
	H.exp(expQ, privKey.q, rootForQ);

	if (rootForQ.cmp(rootForP) == 1) {
		GarnerAlgorithmCRT(privKey.p, privKey.q, rootForP, rootForQ, signature.B);
	} else {
		GarnerAlgorithmCRT(privKey.q, privKey.p, rootForQ, rootForP, signature.B);
	}
}

void ESRabinManager::GarnerAlgorithmCRT(const BigInt &p, const BigInt &q,
					const BigInt &Vp, const BigInt &Vq,
					BigInt &res)
{
	assert(Vq.cmp(Vp) != -1);

	BigInt exponent, tmp, diff;

	exponent.copyContent(q);
	tmp.setNumber(2);
	exponent.sub(tmp);

	tmp.setZero();
	p.exp(exponent, q, tmp);

	diff.copyContent(Vq);
	diff.sub(Vp);

	diff.mulMont(tmp, q, tmp);

	tmp.mulHalfNumbers(p, res);

	res.add(Vp);
}

bool ESRabinManager::checkSignature(const ESRabinSignature &signature,
				    const ESRabinPublicKey &pubKey)
{
	BigInt H, res;
	std::vector<uint8_t> byteArray = signature.R.getByteArray();
	std::vector<uint8_t> bytes(signature.message.begin(), signature.message.end());
	unsigned char digest[SHA256_DIGEST_LENGTH] = {0};

	bytes.insert(bytes.end(), byteArray.begin(), byteArray.end());
	pubKey.hash(bytes.data(), bytes.size(), digest);
	H.fromByteArray(digest, SHA256_DIGEST_LENGTH);

	signature.B.mulMont(signature.B, pubKey.n, res);
	return H.isEqual(res);
}
