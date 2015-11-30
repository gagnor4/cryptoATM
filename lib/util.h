#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

#define MSG_LEN 512

bool read_string(stringstream* stream, string& str);
bool read_int(stringstream* stream, int& i);
bool read_positive_int(stringstream* stream, int& i);

class Socket {
 public:
  Socket(int p);
  int writen(char *msg, int size);
  int readn(char *msg, int size);
 private:
  int port;
  int sockfd;
};

class Server {
 public:
  Server(int p);
  int writen(int cli, char *msg, int size);
  int readn(int cli, char *msg, int size);
  int wait_for_connection();
 private:
  int port;
  int sockfd;
};

#endif
