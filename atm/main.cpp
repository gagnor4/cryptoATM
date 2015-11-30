/*
  main.cpp - ATM
*/

#include "atm.h"
#include "../lib/util.h"

#include <stdlib.h>
#include <iostream>
#include <sstream>

using namespace std;

const string msg = "---ATM---\nInput:";

bool validate(string user, int pin) {
  return true;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Wrong number of arguments\n";
    cout << "Usage: ./atm port" << endl;
    return 1;
  }
  int port = atoi(argv[1]);
  if (port < 1024 || port > 65000) {
    cout << "Invalid port" << endl;
    return 1;
  }
  
  ATM atm(port);
  Session* session = 0;

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
    if (session) {
      if (choice == "login") {
        cout << "Already logged in as user " << session->user << endl;
      }
      else if (choice == "balance") {
        cout << "User balance: $" << atm.balance(session->user) << endl;
      }
      else if (choice == "withdraw") {
        if (!read_positive_int(stream, amount)) continue;
        valid = atm.withdraw(session->user, amount);
        if (valid) {
          cout << "Withdrew $" << amount << endl;
        }
        else {
          cout << "Invalid withdraw" << endl;
        }
      }
      else if (choice == "transfer") {
        if (!read_positive_int(stream, amount)) continue;
        if (!read_string(stream, user)) continue;
        valid = atm.transfer(session->user, user, amount);
        if (valid) {
          cout << "Transfered $" << amount << " from " << session->user <<
            " to " << user << endl;
        }
        else {
          cout << "Invalid transfer" << endl;
        }
      }
      else if (choice == "logout") {
        delete session;
      }
      else {
        cout << "Invalid input" << endl;
      }
    }
    else {
      if (choice == "login") {
        if (!read_string(stream, user)) continue;
        cout << "Enter PIN:" << endl;
        getline(cin, input);
        stream = new stringstream(input);
        if (!read_positive_int(stream, amount)) continue;
        if (validate(user, amount)) {
          session = new Session(user);
        }
        else {
          cout << "Invalid PIN" << endl;
        }
      }
      else if (choice == "balance" || choice == "withdraw" ||
               choice == "transfer" || choice == "logout") {
        cout << "Must login to attempt action" << endl;
      }
      else {
        cout << "Invalid input" << endl;
      }
    }
  }
  return 0;
}
