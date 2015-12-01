/*
  bank.cpp - Bank
*/
#include "bank.h"

#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

// Initialize bank
Bank::Bank(int p) {
  server = new Server(p);
  keypair = create_RSA();
  buffer = (char *)malloc(MSG_LEN * sizeof(char));
  message = (char *)malloc(MSG_LEN * sizeof(char));
  
  database["Alice"] = new Account(100);
  database["Bob"] = new Account(50);
  database["Eve"] = new Account(0);
}

Bank::~Bank() {
  delete server;

  delete buffer;
  delete message;
  map<string, Account*>::iterator itr;

  for (itr = database.begin(); itr != database.end(); ++itr) {
    delete itr->second;
  }
}

// Find account object associated with user,
// Return 0 if user does not exist
Account* Bank::find_user(string user) {
  map<string, Account*>::iterator itr = database.find(user);
  if (itr == database.end()) {
    cout << "Invalid user" << endl;
    return 0;
  }
  return itr->second;
}

// Deposit amount into user's account, accept negative numbers
// Return -1 if user does not exist or does not have enough money
int Bank::deposit(string user, int amount) {
  Account* acc = find_user(user);
  if (acc) {
    if (acc->more_than(amount)) {
      acc->add_money(amount);
      return 1;
    }
    else {
      return 0;
    }
  }
  return -1;
}

// Check balance of user
// Return -1 if user does not exist
int Bank::balance(string user) {
  Account* acc = find_user(user);
  if (acc) {
    return acc->get_money();
  }
  return -1;
}

void Bank::run() {
  socklen_t clilen;
  struct sockaddr_in addr;
  int i;
  
  fd_set read_connections;
  fd_set* connections = server->get_connections();
  while (1) {
    // Block until input arrives on sockets
    read_connections = *connections;
    if (select(FD_SETSIZE, &read_connections, NULL, NULL, NULL) < 0) {
      perror("Could not select connection");
      return;
    }

    // Service all sockets with input pending
    for (i=0; i<FD_SETSIZE; ++i) {
      if (FD_ISSET(i, &read_connections)) {
        // Connection request on server socket
        if (i == server->get_sockfd()) {
          int clisockfd;
          clilen = sizeof(addr);
          clisockfd = accept(server->get_sockfd(), (struct sockaddr*) &addr, &clilen);
          if (clisockfd < 0) {
            perror("Could not accept");
            return;
          }
          FD_SET(clisockfd, connections);
        }
        // Data arriving on already connected socket
        else {
          if (handle_message(i) < 0) {
            close(i);
            FD_CLR(i, connections);
          }
        }
      }
    }
  }
}

int Bank::handle_message(int fd) {
  memset(message, 0, MSG_LEN);
  int bytes = server->readn(fd, message, MSG_LEN);
  if (bytes != MSG_LEN) {
    cout << "Bad read" << endl;
    return -1;
  }
  decrypt_message(message, buffer);
  int response = read_message(buffer);
  create_message(buffer, response);
  encrypt_message(buffer, message);
  bytes = server->writen(fd, message, MSG_LEN);
  if (bytes != MSG_LEN) {
    cout << "Bad write" << endl;
    return -1;
  }
  return bytes;
}


int Bank::encrypt_message(char* from, char* to) {
  memset(to, 0, MSG_LEN);
  memcpy(to, from, MSG_LEN * sizeof(char));
  return true;
}

int Bank::decrypt_message(char* from, char* to) {
  memset(to, 0, MSG_LEN);
  memcpy(to, from, MSG_LEN * sizeof(char));
  return true;
}

int Bank::read_message(char* buf) {
  int out = 0;
  int offset = 0;
  long long time;
  memcpy(&time, &buf[offset], sizeof(time));
  offset += sizeof(time);
  bool type;
  memcpy(&type, &buf[offset], sizeof(type));
  offset += sizeof(type);
  unsigned int userlen;
  memcpy(&userlen, &buf[offset], sizeof(userlen));
  offset += sizeof(userlen);
  string user(&buf[offset], userlen);
  offset += sizeof(user);
  if (type) {
    int amount;
    memcpy(&amount, &buf[offset], sizeof(amount));
    out = (int) deposit(user, amount);
    cout << time << ',' << type << ',' << userlen << ',' << user << ',' << amount << endl;
  }
  else {
    out = balance(user);
    cout << time << ',' << type << ',' << userlen << ',' << user << endl;
  }
  return out;
}

int Bank::create_message(char* buf, int response) {
  memset(buf, 0, MSG_LEN);
  memcpy(&buf[0], &response, sizeof(response));
  return response;
}
