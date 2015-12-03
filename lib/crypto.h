#ifndef CRYPTO_H
#define CRYPTO_H

#include <cryptopp/rsa.h>
#include <cryptopp/hex.h>
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <cryptopp/queue.h>
#include <cryptopp/files.h>
#include <cryptopp/modes.h>

#include <sstream>

using namespace std;
using namespace CryptoPP;

#define KEY_LEN 1024
#define EXP 17
#define MSG_LEN (KEY_LEN/8)
#define MAX_STR (MSG_LEN/4)

extern const char valid_message[16];

void SavePrivateKey(const string& filename, const PrivateKey& key);
void SavePublicKey(const string& filename, const PublicKey& key);
void Save(const string& filename, const BufferedTransformation& bt);

void LoadPrivateKey(const string& filename, PrivateKey& key);
void LoadPublicKey(const string& filename, PublicKey& key);
void Load(const string& filename, BufferedTransformation& bt);

void save_key_to_buffer(const RSA::PublicKey& key, char* buffer);
void load_key_from_buffer(RSA::PublicKey& key, char* buffer);

void RSA_encrypt(char* from, char* to, RSA::PublicKey& key);
void RSA_decrypt(char* from, char* to, RSA::PrivateKey& key);

void AES_encrypt(char* buffer, int size, byte* key, byte* iv);
void AES_decrypt(char* buffer, int size, byte* key, byte* iv);

void generate_aes_key(byte* key, byte* iv);

#endif
