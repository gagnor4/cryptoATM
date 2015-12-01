#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

// Net
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// Crypto
#include <openssl/rsa.h>
#include <openssl/rand.h>

using namespace std;

#define MSG_LEN 512
#define MAX_STR (MSG_LEN/2)
#define PADDING RSA_PKCS1_PADDING
#define KEY_LEN 1024
#define KEY_EXP 65537

// Convenience
bool read_string(stringstream* stream, string& str);
bool read_int(stringstream* stream, int& i);
bool read_positive_int(stringstream* stream, int& i);

long long get_current_time();

// Networking
class Socket {
 public:
  Socket(int p);
  ~Socket();
  int writen(char *msg, int size);
  int readn(char *msg, int size);
 private:
  int port;
  int sockfd;
};

class Server {
 public:
  Server(int p);
  ~Server();
  int writen(int cli, char *msg, int size);
  int readn(int cli, char *msg, int size);
  int read_buffer(int cli);
  fd_set* get_connections();
  int get_sockfd();
 private:
  int port;
  int sockfd;
  fd_set connections;
  char* buf;
};

// Crypto
void initialize_crypto();
RSA* create_RSA();
int public_encrypt(unsigned char* from, unsigned char* to, RSA* rsa);
int private_decrypt(unsigned char* from, unsigned char* to, RSA* rsa);

#endif
