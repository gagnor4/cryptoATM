/*
  bank.cpp - Bank
*/
#include "bank.h"

Bank::Bank(int p) {
  server = new Server(p);

  database["Alice"] = new Account(100);
  database["Bob"] = new Account(50);
  database["Eve"] = new Account(0);
}

bool Bank::deposit(string user, int amount) {
  Account* acc = find_user(user);
  if (acc) {
    acc->add_money(amount);
    return true;
  }
  return false;
}

int Bank::balance(string user) {
  Account* acc = find_user(user);
  if (acc) {
    return acc->get_money();
  }
  return -1;
}

Account* Bank::find_user(string user) {
  map<string, Account*>::iterator itr = database.find(user);
  if (itr == database.end()) {
    cout << "Invalid user" << endl;
    return 0;
  }
  return itr->second;
}
