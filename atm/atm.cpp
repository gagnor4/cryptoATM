/*
  atm.cpp - ATM
*/
#include "atm.h"

ATM::ATM(int p) {
  socket = new Socket(p);
}

bool ATM::transfer(string from, string to, int amount) {
  
}

bool ATM::withdraw(string user, int amount) {
  
}

int ATM::balance(string user) {
  
}
