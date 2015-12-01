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
  
  database["Alice"] = new Account(100);
  database["Bob"] = new Account(50);
  database["Eve"] = new Account(0);
}

// Deposit amount into user's account, accept negative numbers
// Return false if user does not exist
bool Bank::deposit(string user, int amount) {
  Account* acc = find_user(user);
  if (acc) {
    acc->add_money(amount);
    return true;
  }
  return false;
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
  /*
  while (1) {
    if (connections.size() > 0) {
      for (int i=0; i<connections.size(); ++i) {
        int count = 0;
        ioctl(connections[i], FIONREAD, &count);
        if (count > 0) {
          process_packet(connections[i]);
        }
      }
    }
    }*/
  int bytes = 0;
  int cli = server->wait_for_connection();
  char *buffer = (char *)malloc(MSG_LEN * sizeof(char));
  bzero(buffer, MSG_LEN);
  while (1) {
    bytes = server->readn(cli, buffer, MSG_LEN);
    if (bytes != MSG_LEN) {
      cout << "Bad read" << endl;
      continue;
    }
    int msg = decode_message(buffer);
    encode_and_send(msg);
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

// Message starts with bool
// Deposit = 1, Balance = 0
// Followed by unsigned int for string length
// Then the string
// Then final int for amount if deposit, otherwise padding
int Bank::decode_message(char *buffer) {
  char *message = (char *)malloc(MSG_LEN * sizeof(char));
  bool valid = decrypt_message(buffer, message);
  int out = -1;
  if (!valid) {
    cout << "Invalid message" << endl;
    return -1;
  }
  else {
    int offset = 0;
    bool type;
    memcpy(type, message[offset], sizeof(type));
    offset += sizeof(type);
    unsigned int userlen;
    memcpy(userlen, message[offset], sizeof(userlen));
    offset += sizeof(userlen);
    string user(message[offset], userlen);
    offset += sizeof(user);
    if (type) {
      int amount;
      memcpy(amount, message[offset], sizeof(amount));
      out = (int) deposit(user, amount);
    }
    else {
      out = balance(user);
    }
  }
  return out;
}

void Bank::encode_and_send(int msg) {
  
}
