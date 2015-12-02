/*
  util.cpp - Utilities
*/

#include "util.h"

using namespace std;

// Convenience function for reading string from stream
bool read_string(stringstream* stream, string& str) {
  if (*stream >> str) {
    if (str.length() < MAX_STR) {
      return true;
    }
  }
  cout << "Invalid string" << endl;
  return false;  
}

// Convenience function for reading int from stream
bool read_int(stringstream* stream, int& i) {
  if (*stream >> i) {
    return true;
  }
  cout << "Invalid integer" << endl;
  return false;
}

// Convenience function for reading positive int from stream
bool read_positive_int(stringstream* stream, int& i) {
  if (*stream >> i) {
    if (i > 0) {
      return true;
    }
  }
  cout << "Invalid integer" << endl;
  return false;
}

// Return current time in milliseconds
long long get_current_time() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000;
}

void print_buffer(char* buf) {
  for (int i=0; i<MSG_LEN; ++i) {
    cout << int(buf[i]) << " ";
  }
  cout << endl;
}

// Socket initialization
Socket::Socket(int p) {
  port = p;

  struct sockaddr_in addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("Failed to open socket");
    return;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr));

  if (connect(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
    perror("Failed to connect");
    return;
  }
}

Socket::~Socket() {
  close(sockfd);
}

// Write message from buffer of size
int Socket::writen(char *msg, int size) {
  int n = write(sockfd, msg, size);
  if (n != size) {
    perror("Failed to write");
  }
  return n;
}

// Read message to buffer of size
int Socket::readn(char *msg, int size) {
  int n = read(sockfd, msg, size);
  if (n != size) {
    perror("Failed to read");
  }
  return n;
}

// Server initialization
Server::Server(int p) {
  port = p;
  
  struct sockaddr_in addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("Failed to open socket");
    return;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr));

  if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
    perror("Failed to bind");
    return;
  }
  listen(sockfd, 5);

  FD_ZERO(&connections);
  FD_SET(sockfd, &connections);
}

Server::~Server() {
  for (int i=0; i<FD_SETSIZE; ++i) {
    close(i);
  }
  close(sockfd);
}

// Write buffer of size to client
int Server::writen(int cli, char *msg, int size) {
  int n = write(cli, msg, size);
  if (n != size) {
    perror("Failed to write");
  }
  return n;
}

// Read buffer of size from client
int Server::readn(int cli, char *msg, int size) {
  int n = read(cli, msg, size);
  if (n != size) {
    perror("Failed to read");
  }
  return n;
}

fd_set* Server::get_connections() {
  return &connections;
}

int Server::get_sockfd() {
  return sockfd;
}


User::User(string _name, Integer n, Integer e, Integer d) {
  name = _name;
  key.Initialize(n, d, e);
  pub.Initialize(n, d);
}

RSA::PrivateKey User::get_private_key() {
  return key;
}

RSA::PublicKey User::get_public_key() {
  return pub;
}
