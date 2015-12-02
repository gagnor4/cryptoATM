#ifndef ATM_H
#define ATM_H

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
  ~ATM();
  int transfer(string from, string to, int amount);
  int withdraw(string user, int amount);
  int balance(string user);
 private:
  void connect_to_bank();
  int create_query(char* buf);
  int create_message(bool type, string user, int amount, char* buf);
  int read_message(char* buf);
  int encrypt_message(char* from, char* to);
  int decrypt_message(char* from, char* to);
  bool send_message();
  bool wait_for_message();

  Socket* socket;
  RSA::PrivateKey atmkey;
  RSA::PublicKey atmpub;
  RSA::PublicKey bankpub;
  RSA::PrivateKey bankkey;
  char* buffer;
  char* message;
};

#endif
