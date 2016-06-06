#include <openssl/sha.h>
#include "ESRabin.h"
#include "logger.h"

void ESRabinManager::generateKeys(ESRabinPublicKey &pubKey, ESRabinPrivateKey &privKey)
{
	pubKey.n.generateBlumPrime(privKey.p, privKey.q);
	pubKey.hash = SHA256;
	pubKey.nameHashFunc.assign("SHA256");
}

void ESRabinManager::signMessage(const std::string &message, ESRabinSignature &signature,
		 const ESRabinPublicKey &pubKey,
		 const ESRabinPrivateKey &privKey)
{
	signature.message.assign(message);
	signature.R.generateRand(768);
	signature.B.generateRand();
}

bool ESRabinManager::checkSignature(ESRabinSignature &signature)
{
	INFO("checkSignature: B = {}", signature.getB().toString());
	INFO("checkSignature: R = {}", signature.getR().toString());
	INFO("checkSignature: message = {}", signature.getMessage().c_str());
	return true;
}
