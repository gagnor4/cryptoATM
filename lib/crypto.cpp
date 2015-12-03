/*
  crypto.cpp - Crypto
*/

#include "crypto.h"

using namespace std;

const char valid_message[] = "VALIDMESSAGE123";

void SavePrivateKey(const string& filename, const PrivateKey& key)
{
	// http://www.cryptopp.com/docs/ref/class_byte_queue.html
	ByteQueue queue;
	key.Save(queue);

	Save(filename, queue);
}

void SavePublicKey(const string& filename, const PublicKey& key)
{
	// http://www.cryptopp.com/docs/ref/class_byte_queue.html
	ByteQueue queue;
	key.Save(queue);

	Save(filename, queue);
}

void Save(const string& filename, const BufferedTransformation& bt)
{
	// http://www.cryptopp.com/docs/ref/class_file_sink.html
	FileSink file(filename.c_str());

	bt.CopyTo(file);
	file.MessageEnd();
}

void LoadPrivateKey(const string& filename, PrivateKey& key)
{
	// http://www.cryptopp.com/docs/ref/class_byte_queue.html
	ByteQueue queue;

	Load(filename, queue);
	key.Load(queue);	
}

void LoadPublicKey(const string& filename, PublicKey& key)
{
	// http://www.cryptopp.com/docs/ref/class_byte_queue.html
	ByteQueue queue;

	Load(filename, queue);
	key.Load(queue);	
}

void Load(const string& filename, BufferedTransformation& bt)
{
	// http://www.cryptopp.com/docs/ref/class_file_source.html
	FileSource file(filename.c_str(), true /*pumpAll*/);

	file.TransferTo(bt);
	bt.MessageEnd();
}

void save_key_to_buffer(const RSA::PublicKey& key, char* buffer) {
  memset(buffer, 0, MSG_LEN);
  Integer n = key.GetModulus();
  n.Encode((byte *)buffer, MSG_LEN);
}

void load_key_from_buffer(RSA::PublicKey& key, char* buffer) {
  Integer n((const byte*)buffer, MSG_LEN);
  key.Initialize(n, EXP);
}

void RSA_encrypt(char* from, char* to, RSA::PublicKey& key) {
  memset(to, 0, MSG_LEN);
  Integer m((const byte*)from, MSG_LEN);
  Integer c = key.ApplyFunction(m);
  c.Encode((byte*)to, MSG_LEN);
}

void RSA_decrypt(char* from, char* to, RSA::PrivateKey& key) {
  memset(to, 0, MSG_LEN);
  AutoSeededRandomPool rng;
  Integer c((const byte*)from, MSG_LEN);
  Integer r = key.CalculateInverse(rng, c);
  r.Encode((byte*)to, MSG_LEN);
}

void AES_encrypt(char* buffer, int size, byte* key, byte* iv) {
  CFB_Mode<AES>::Encryption enc(key, AES::DEFAULT_KEYLENGTH, iv);
  enc.ProcessData((byte*) buffer, (byte*) buffer, size - 16);
}

void AES_decrypt(char* buffer, int size, byte* key, byte* iv) {
  CFB_Mode<AES>::Decryption dec(key, AES::DEFAULT_KEYLENGTH, iv);
  dec.ProcessData((byte*) buffer, (byte*) buffer, size - 16);
}
  

void generate_aes_key(byte* key, byte* iv) {
  memset(key, 0, AES::DEFAULT_KEYLENGTH);
  memset(iv, 0, AES::BLOCKSIZE);
  AutoSeededRandomPool rnd;
  rnd.GenerateBlock(key, AES::DEFAULT_KEYLENGTH);
  rnd.GenerateBlock(iv, AES::BLOCKSIZE);
}
