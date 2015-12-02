// Linux help: http://www.cryptopp.com/wiki/Linux

// Debug:
// g++ -g -ggdb -O0 -Wall -Wextra -Wno-unused -Wno-type-limits -I. -I/usr/include/cryptopp cryptopp-key-gen.cpp -o cryptopp-key-gen.exe -lcryptopp

// Release:
// g++ -O2 -Wall -Wextra -Wno-unused -Wno-type-limits -I. -I/usr/include/cryptopp cryptopp-key-gen.cpp -o cryptopp-key-gen.exe -lcryptopp && strip --strip-all cryptopp-key-gen.exe

#include "crypto.h"

#include <stdexcept>

int main(int argc, char** argv)
{
	std::ios_base::sync_with_stdio(false);

	// http://www.cryptopp.com/docs/ref/class_auto_seeded_random_pool.html
	AutoSeededRandomPool rnd;

	try
	{
		// http://www.cryptopp.com/docs/ref/rsa_8h.html
		RSA::PrivateKey rsaPrivate;
		rsaPrivate.GenerateRandomWithKeySize(rnd, 1024);

		RSA::PublicKey rsaPublic(rsaPrivate);

    if(!rsaPrivate.Validate(rnd, 3)) {
      throw runtime_error("Validation failed");
    }

		SavePrivateKey("rsa-private.key", rsaPrivate);
		SavePublicKey("rsa-public.key", rsaPublic);

		cout << "Successfully generated and saved RSA keys:" << endl;
    cout << "Values:" << endl;
    cout << "N: " << rsaPrivate.GetModulus() << endl;
    cout << "E: " << rsaPrivate.GetPublicExponent() << endl;
    cout << "D: " << rsaPrivate.GetPrivateExponent() << endl;
	}

	catch(CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		return -2;
	}

	catch(std::exception& e)
	{
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}
