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
				break;
			}
		}
		LOG("Number '{}' is not square lyshok", H.toString());
		LOG("Size of bytes is {}", bytes.size());
//		for (int i = 0; i < bytes.size(); ++i) {
//			LOG("byte[{}] = {:X}", i, bytes[i]);
//		}
//		LOG("B = {}", signature.B.toString());

		// signature.B
	}
	// calculate B
	signature.B.generateRand();
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
