#ifndef BANK_H
#define BANK_H

#include "../lib/util.h"

using namespace std;

class Session {
 public:
  Session(string s) {user = s;}
  string user;
};

class ATM {
 public:
  ATM(int p);
  bool transfer(string from, string to, int amount);
  bool withdraw(string user, int amount);
  int balance(string user);
 private:
  void connect_to_bank();
  Socket* socket;
  RSA* keypair;
};

#endif
