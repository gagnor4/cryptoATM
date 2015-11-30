#ifndef BANK_H
#define BANK_H

#include "../lib/util.h"

#include <map>

using namespace std;

class Account {
public:
  Account(int m) {money = m;}
  int get_money() {return money;}
  void set_money(int m) {money = m;}
  void add_money(int m) {money += m;}
private:
  int money;
};

class Bank {
public:
  Bank(int p);
  bool deposit(string user, int amount);
  int balance(string user);
private:
  Account* find_user(string user);
  
  Server* server;
  map<string, Account*> database;
};

#endif
