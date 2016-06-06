#include <iostream>
#define ALLOCATE_LOGGER
#include "logger/logger.h"
#undef ALLOCATE_LOGGER

#include "include/BigInt.h"
#include "ESRabin.h"

int main() {

	ESRabinManager manager;
	ESRabinPublicKey pubKey;
	ESRabinPrivateKey privKey;
	ESRabinSignature sign;

	std::string msg("Hello, World!");

	manager.generateKeys(pubKey, privKey);
	manager.signMessage(msg, sign, pubKey, privKey);
	if (manager.checkSignature(sign)) {
		INFO("Check function checked signature!");
	} else {
		INFO("Check function: signature wrong!");
	}

	return 0;
}
