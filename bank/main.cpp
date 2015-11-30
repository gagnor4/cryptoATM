/*
  main.cpp - Bank
*/

#include "bank.h"
#include "../lib/util.h"

#include <stdlib.h>
#include <iostream>

using namespace std;

const string msg = "---BANK---\nInput:";

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Wrong number of arguments\n";
    cout << "Usage: ./bank port" << endl;
    return 1;
  }
  int port = atoi(argv[1]);
  if (port < 1024 || port > 65000) {
    cout << "Invalid port" << endl;
    return 1;
  }
  
  Bank bank(port);
  
  

  cout << msg << endl;
  string input, choice, user;
  int amount;
  bool valid;
  stringstream* stream;
  while(1) {
    input = "";
    choice = "";
    user = "";
    amount = 0;
    valid = false;
    getline(cin, input);
    stream = new stringstream(input);
    if (!read_string(stream, choice)) continue;
    if (choice == "deposit") {
      if (!read_string(stream, user)) continue;
      if (!read_positive_int(stream, amount)) continue;
      if (bank.deposit(user, amount)) {
        cout << "Deposited $" << amount << " to " << user << endl;
      }
      else {
        cout << "Invalid deposit" << endl;
      }
    }
    else if (choice == "balance") {
      if (!read_string(stream, user)) continue;
      int bal = bank.balance(user);
      if (bal >= 0) {
        cout << "Balance: $" << bal << endl;
      }
      else {
        cout << "Unknown user" << endl;
      }
    }
    else if (choice == "exit") {
      break;
    }
    else {
      cout << "Invalid input" << endl;
    }
  }
  return 0;
}
