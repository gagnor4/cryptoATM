/*
  main.cpp - Bank
*/

#include "bank.h"
#include "../atm/atm.h"
#include "../lib/util.h"

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
  
  pid_t pid;
  pid = fork();

  if (pid < 0) {
    perror("Fork failed");
    return 1;
  }

  if (pid == 0) {
    Bank bank(port); 
    bank.run();
  }
  else {
    sleep(1);
    ATM atm(port);
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
      // Read choice
      if (!read_string(stream, choice)) continue;
      if (choice == "deposit") {
        if (!read_string(stream, user)) continue;
        if (!read_positive_int(stream, amount)) continue;
        ret = atm.withdraw(user, 0-amount);
        if (ret > 0) {
          cout << "Deposited $" << amount << " to " << user << endl;
        }
        else if (ret == 0) {
          cout << "Insufficient funds" << endl;
        }
        else {
          cout << "Invalid user" << endl;
        }
      }
      else if (choice == "balance") {
        if (!read_string(stream, user)) continue;
        int bal = atm.balance(user);
        if (bal >= 0) {
          cout << "Balance: $" << bal << endl;
        }
        else {
          cout << "Invalid user" << endl;
        }
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
