/*
  util.cpp - Utilities
*/

#include "util.h"

using namespace std;

bool read_string(stringstream* stream, string& str) {
  if (*stream >> str) {
    return true;
  }
  cout << "Invalid input" << endl;
  return false;  
}

bool read_int(stringstream* stream, int& i) {
  if (*stream >> i) {
    return true;
  }
  cout << "Invalid input" << endl;
  return false;
}

bool read_positive_int(stringstream* stream, int& i) {
  if (*stream >> i) {
    if (i > 0) {
      return true;
    }
  }
  cout << "Invalid input" << endl;
  return false;
}


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

int Socket::writen(char *msg, int size) {
  int n = write(sockfd, msg, size);
  if (n != size) {
    perror("Failed to write");
  }
  return n;
}

int Socket::readn(char *msg, int size) {
  int n = read(sockfd, msg, size);
  if (n != size) {
    perror("Failed to read");
  }
  return n;
}

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
}

int Server::writen(int cli, char *msg, int size) {
  int n = write(cli, msg, size);
  if (n != size) {
    perror("Failed to write");
  }
  return n;
}

int Server::readn(int cli, char *msg, int size) {
  int n = read(cli, msg, size);
  if (n != size) {
    perror("Failed to read");
  }
  return n;
}

int Server::wait_for_connection() {
  socklen_t clilen;
  struct sockaddr_in addr;
  clilen = sizeof(addr);
  int newsockfd = accept(sockfd, (struct sockaddr*) &addr, &clilen);
  if (newsockfd < 0) {
    perror("Failed to accept");
    return -1;
  }
  return newsockfd;
}
