#ifndef BANK_H
#define BANK_H

#include "../lib/util.h"

#include <vector>
#include <map>

using namespace std;

class Account {
public:
  Account(int m) {money = m;}
  int get_money() {return money;}
  void set_money(int m) {money = m;}
  int add_money(int m) {money += m; return money;}
  bool more_than(int m);
  
  byte key[AES::DEFAULT_KEYLENGTH];
  byte iv[AES::BLOCKSIZE];
private:
  int money;
};

class Bank {
public:
  Bank(int p);
  ~Bank();
  void run();
private:
  void new_atm(int fd);
  void remove_atm(int fd);
  Session* find_atm_session(int fd);
  bool authenticate_atm(int fd, char* from, char* to);

  Account* find_account(string name);
  bool authenticate_user(Session* session, char* from, char* to);
  
  int read(int fd);
  void read_header(Session* session, char* from, char* to);
  void read_body(Session* session, int offset, char* from ,char* to);
  bool valid_timestamp(Session* s, long long t);

  void create_message(int response, char* buf);
  
  int transaction(string name, int amount);
  bool valid_timestamp(long long t);

  bool send_message(int fd, char* buf);
  bool recv_message(int fd, char* buf);
  
  Server* server;
  RSA::PrivateKey bankkey;
  char* buffer;
  char* message;
  map<string, Account*> database;
  map<int, Session*> atmkeyring;
};

#endif
