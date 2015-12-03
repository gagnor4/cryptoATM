#ifndef ATM_H
#define ATM_H

#include "../lib/util.h"

using namespace std;

class User {
 public:
  User(string s, Integer n, Integer e, Integer d);
  User(string s, RSA::PrivateKey k, RSA::PublicKey p);
  string get_name() {return name;}
  RSA::PrivateKey get_private_key() {return pr;}
  RSA::PublicKey get_public_key() {return pub;}
  
  byte key[AES::DEFAULT_KEYLENGTH];
  byte iv[AES::BLOCKSIZE];
 private:
  string name;
  RSA::PrivateKey pr;
  RSA::PublicKey pub;
};

class ATM {
 public:
  ATM(int p);
  ~ATM();
  bool login(string name, int pin);
  bool login(string name);
  bool loggedin();
  string get_username();
  void logout();

  int balance();
  int withdraw(int amount);
  int transfer(string name, int amount);
 private:
  bool authenticate_atm();
  bool authenticate_user();

  int send(string name, int amount);
  
  int create_message(string name, int amount, char* buf);
  int create_header(bool auth, char* buf);
  int create_body(string name, int amount, int offset, char* buf);

  int read_message(char* buf);
  int read_header(char* buf);
  int read_body(char* buf);
  bool valid_timestamp(long long t);

  bool send_message(char* buf);
  bool recv_message(char* buf);

  Socket* socket;
  RSA::PrivateKey atmkey;
  RSA::PublicKey atmpub;
  RSA::PublicKey bankpub;

  User* user;
  Session* session;
  char* buffer;
  char* message;
};

#endif
