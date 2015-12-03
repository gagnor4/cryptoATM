#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>

// Net
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// Crypto
#include "crypto.h"

using namespace std;

// Stream reading functions
bool read_string(stringstream* stream, string& str);
bool read_int(stringstream* stream, int& i);
bool read_positive_int(stringstream* stream, int& i);
bool read_Integer(stringstream* stream, Integer& i);

long long get_current_time();

void pad(int amount, char* buf);
void print_buffer(char* buf);

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
};

struct Session {
  byte key[AES::DEFAULT_KEYLENGTH];
  byte iv[AES::BLOCKSIZE];
  RSA::PublicKey pub;
  string auth;
  long long time;
};

#endif
