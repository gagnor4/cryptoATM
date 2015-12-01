/*
  main.cpp - ATM
*/

#include "atm.h"
#include "../lib/util.h"

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
  int amount, ret;
  stringstream* stream;
  while(1) {
    input = "";
    choice = "";
    user = "";
    amount = 0;
    getline(cin, input);
    stream = new stringstream(input);
    if (!read_string(stream, choice)) continue;
    if (session) {
      if (choice == "login") {
        cout << "Already logged in as user " << session->user << endl;
      }
      else if (choice == "balance") {
        ret = atm.balance(session->user);
        if (ret >= 0) {
          cout << "User balance: $" << ret << endl;
        }
        else {
          cout << "Invalid user" << endl;
        }
      }
      else if (choice == "withdraw") {
        if (!read_positive_int(stream, amount)) continue;
        ret = atm.withdraw(session->user, amount);
        if (ret > 0) {
          cout << "Withdrew $" << amount << endl;
        }
        else if (ret == 0) {
          cout << "Insufficient funds" << endl;
        }
        else {
          cout << "Invalid user" << endl;
        }
      }
      else if (choice == "transfer") {
        if (!read_positive_int(stream, amount)) continue;
        if (!read_string(stream, user)) continue;
        ret = atm.transfer(session->user, user, amount);
        if (ret > 0) {
          cout << "Transfered $" << amount << " from " << session->user <<
            " to " << user << endl;
        }
        else if (ret == 0) {
          cout << "Insufficient funds" << endl;
        }
        else {
          cout << "Invalid user" << endl;
        }
      }
      else if (choice == "logout") {
        delete session;
        session = 0;
      }
      else if (choice == "exit") {
        delete session;
        break;
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
      else if (choice == "exit") {
        break;
      }
      else {
        cout << "Invalid input" << endl;
      }
    }
  }
  return 0;
}
