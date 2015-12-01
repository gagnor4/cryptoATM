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
  void add_money(int m) {money += m;}
  bool more_than(int m) {return (m >= 0 || money + m >= 0);}
private:
  int money;
};

class Bank {
public:
  Bank(int p);
  ~Bank();
  void run();
private:
  Account* find_user(string user);

  int deposit(string user, int amount);
  int balance(string user);

  int handle_message(int fd);
  int encrypt_message(char* from, char* to);
  int decrypt_message(char* from, char* to);
  int read_message(char* buf);
  int create_message(char* buf, int response);
  
  Server* server;
  RSA* keypair;
  char* buffer;
  char* message;
  map<string, Account*> database;
};

#endif
