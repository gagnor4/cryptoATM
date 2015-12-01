/*
  atm.cpp - ATM
*/
#include "atm.h"

ATM::ATM(int p) {
  socket = new Socket(p);
  keypair = create_RSA();

  connect_to_bank();
}

bool ATM::transfer(string from, string to, int amount) {
  
}

bool ATM::withdraw(string user, int amount) {
  
}

int ATM::balance(string user) {
  
}


void ATM::connect_to_bank() {
  char* buffer;
  int b = encode_message(true, "Alice", 10, buffer);
  char* message = (char *)malloc(MSG_LEN * sizeof(char));
  bzero(message, MSG_LEN);
  memcpy(message, buffer, b);
  socket->writen(message, MSG_LEN);
}

int encode_message(bool type, string user, int amount, char* buffer) {
  unsigned int userlen = user.length();
  int size = sizeof(int) + sizeof(userlen) + userlen + sizeof(amount);
  buffer = (char *)malloc(size);
  int offset = 0;
  memcpy(buffer[offset], type, sizeof(type));
  offset += sizeof(type);
  memcpy(buffer[offset], userlen, sizeof(userlen));
  offset += sizeof(userlen);
  memcpy(buffer[offset], user, sizeof(user));
  offset += sizeof(user)
  if (type) {
    memcpy(buffer[offset], amount, sizeof(amount));
    offset += sizeof(amount);
  }
  return offset;
}
