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
#include "crypto.h"

using namespace std;

// Convenience
bool read_string(stringstream* stream, string& str);
bool read_int(stringstream* stream, int& i);
bool read_positive_int(stringstream* stream, int& i);

long long get_current_time();

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

class User {
 public:
  User(string _name, Integer n, Integer e, Integer d);
  RSA::PrivateKey get_private_key();
  RSA::PublicKey get_public_key();
 private:
  string name;
  RSA::PrivateKey key;
  RSA::PublicKey pub;
}

#endif
