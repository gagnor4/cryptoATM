#ifndef CRYPTO_H
#define CRYPTO_H


#include <cryptopp/rsa.h>
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <cryptopp/queue.h>
#include <cryptopp/files.h>

using namespace std;
using namespace CryptoPP;

#define MSG_LEN 128
#define MAX_STR (MSG_LEN/2)
#define KEY_LEN 1024

void SavePrivateKey(const string& filename, const PrivateKey& key);
void SavePublicKey(const string& filename, const PublicKey& key);
void Save(const string& filename, const BufferedTransformation& bt);

void LoadPrivateKey(const string& filename, PrivateKey& key);
void LoadPublicKey(const string& filename, PublicKey& key);
void Load(const string& filename, BufferedTransformation& bt);

void encrypt(char* from, char* to, RSA::PublicKey& key);
void decrypt(char* from, char* to, RSA::PrivateKey& key);

#endif
