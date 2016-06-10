#include <iostream>
#define ALLOCATE_LOGGER
#include "logger/logger.h"
#undef ALLOCATE_LOGGER

#include "include/BigInt.h"
#include "ESRabin.h"

int main()
{
	ESRabinManager manager(RandomGeneratorMush::getGeneratorMush());
	ESRabinPublicKey pubKey;
	ESRabinPrivateKey privKey;
	ESRabinSignature signature;

	std::string msg("Hello, World!");

	manager.generateKeys(pubKey, privKey);
	manager.signMessage(msg, signature, pubKey, privKey);

	INFO("Public key data:");
	INFO("\t N = {}", pubKey.getN().toString());
	INFO("\t hash = {}", pubKey.getHash().c_str());

	INFO("Private key data:");
	INFO("\t P = {}", privKey.getP().toString());
	INFO("\t Q = {}", privKey.getQ().toString());

	INFO("Signature data:");
	INFO("\t B = {}", signature.getB().toString());
	INFO("\t R = {}", signature.getR().toString());
	INFO("\t message = {}", signature.getMessage().c_str());

	INFO("");
	if (manager.checkSignature(signature, pubKey)) {
		INFO("Signature is correct!");
	} else {
		INFO("Signature is wrong!");
	}
	manager.finalizeKeys(pubKey, privKey);

	return 0;
}
