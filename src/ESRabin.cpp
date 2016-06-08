#include <openssl/sha.h>
#include "ESRabin.h"
#include "logger.h"

void ESRabinManager::generateKeys(ESRabinPublicKey &pubKey, ESRabinPrivateKey &privKey)
{
	pubKey.n.generateBlumPrime(privKey.p, privKey.q);
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
		signature.R.generateRand();
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
	signature.B.generateRand(700);
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

	DEBUG("root for P = {}", rootForP.toString());
	DEBUG("root for Q = {}", rootForQ.toString());

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

	INFO("checkSignature: B = {}", signature.getB().toString());
	INFO("checkSignature: R = {}", signature.getR().toString());
	INFO("checkSignature: message = {}", signature.getMessage().c_str());
	INFO("checkSignature: H = {}", H.toString());
	INFO("checkSignature: res = {}", res.toString());

	return H.isEqual(res);
}
