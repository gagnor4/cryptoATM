/*
  atm.cpp - ATM
*/
#include "atm.h"

#include <string.h>

ATM::ATM(int p) {
  socket = new Socket(p);
  keypair = create_RSA();

  buffer = new char[MSG_LEN];
  message = new char[MSG_LEN];

  connect_to_bank();
}

ATM::~ATM() {
  delete socket;

  delete[] buffer;
  delete[] message;
}

int ATM::transfer(string from, string to, int amount) {
  create_message(true, from, 0-amount, buffer);
  encrypt_message(buffer, message);
  send_message();
  wait_for_message();
  decrypt_message(message, buffer);
  int response = read_message(buffer);
  if (response > 0) {
    create_message(true, to, amount, buffer);
    encrypt_message(buffer, message);
    send_message();
    wait_for_message();
    decrypt_message(message, buffer);
    return read_message(buffer);
  }
  else {
    return response;
  }
}

int ATM::withdraw(string user, int amount) {
  create_message(true, user, 0-amount, buffer);
  encrypt_message(buffer, message);
  send_message();
  wait_for_message();
  decrypt_message(message, buffer);
  return read_message(buffer);
}

int ATM::balance(string user) {
  create_message(false, user, 0, buffer);
  encrypt_message(buffer, message);
  send_message();
  wait_for_message();
  decrypt_message(message, buffer);
  return read_message(buffer);
}


void ATM::connect_to_bank() {
  
}

int ATM::create_message(bool type, string user, int amount, char* buf) {
  memset(buf, 0, MSG_LEN);
  unsigned int userlen = user.length();
  int offset = 0;
  long long time = get_current_time();
  memcpy(&buf[offset], &time, sizeof(time));
  offset += sizeof(time);
  memcpy(&buf[offset], &type, sizeof(type));
  offset += sizeof(type);
  memcpy(&buf[offset], &userlen, sizeof(userlen));
  offset += sizeof(userlen);
  memcpy(&buf[offset], user.data(), userlen);
  offset += sizeof(user);
  if (type) {
    memcpy(&buf[offset], &amount, sizeof(amount));
    offset += sizeof(amount);
  }
  cout << time << ',' << type << ',' << userlen << ',' << user << ',' << amount << endl;
  return offset;
}

int ATM::read_message(char* buf) {
  int ret;
  memcpy(&ret, &buf[0], sizeof(ret));
  return ret;
}

int ATM::encrypt_message(char* from, char* to) {
  memset(to, 0, MSG_LEN);
  memcpy(to, from, MSG_LEN * sizeof(char));
  return true;
}

int ATM::decrypt_message(char* from, char* to) {
  memset(to, 0, MSG_LEN);
  memcpy(to, from, MSG_LEN * sizeof(char));
  return true;
}

bool ATM::send_message() {
  if (socket->writen(message, MSG_LEN) != MSG_LEN) {
    cout << "Failed write" << endl;
    return false;
  }
  return true;
}

bool ATM::wait_for_message() {
  memset(message, 0, MSG_LEN);
  if (socket->readn(message, MSG_LEN) != MSG_LEN) {
    cout << "Failed read" << endl;
    return false;
  }
  return true;
}
