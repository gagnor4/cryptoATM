/*
  main.cpp - ATM
*/

#include "atm.h"
#include "../lib/util.h"

using namespace std;

const string msg = "---ATM---\nInput:";

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

  cout << msg << endl;
  string input, choice, name;
  int amount, ret;
  stringstream* stream;
  while(1) {
    input = "";
    choice = "";
    name = "";
    amount = 0;
    getline(cin, input);
    stream = new stringstream(input);
    if (!read_string(stream, choice)) continue;
    if (atm.loggedin()) {
      if (choice == "login") {
        cout << "Already logged in as " << atm.get_username() << endl;
      }
      else if (choice == "balance") {
        ret = atm.balance();
        if (ret >= 0) {
          cout << "User balance: $" << ret << endl;
        }
        else {
          cout << "Invalid command" << endl;
        }
      }
      else if (choice == "withdraw") {
        if (!read_positive_int(stream, amount)) continue;
        ret = atm.withdraw(amount);
        if (ret >= 0) {
          cout << "Withdrew $" << amount << endl;
        }
        else {
          cout << "Invalid command" << endl;
        }
      }
      else if (choice == "transfer") {
        if (!read_positive_int(stream, amount)) continue;
        if (!read_string(stream, name)) continue;
        ret = atm.transfer(name, amount);
        if (ret >= 0) {
          cout << "Transfered $" << amount << " from " << atm.get_username() <<
            " to " << name << endl;
        }
        else {
          cout << "Invalid command" << endl;
        }
      }
      else if (choice == "logout") {
        atm.logout();
      }
      else if (choice == "exit") {
        atm.logout();
        break;
      }
      else {
        cout << "Invalid input" << endl;
      }
    }
    else {
      if (choice == "login") {
        if (!read_string(stream, name)) continue;
        delete stream;
        cout << "Enter PIN:" << endl;
        getline(cin, input);
        stream = new stringstream(input);
        if (!read_positive_int(stream, amount)) continue;
        if (!atm.login(name, amount)) {
          cout << "Invalid PIN" << endl;
        }
        else {
          cout << "Welcome " << name << endl;
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
    delete stream;
  }

  if (stream) delete stream;
  return 0;
}
