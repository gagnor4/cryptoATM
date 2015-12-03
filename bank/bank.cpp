/*
  bank.cpp - Bank
*/
#include "bank.h"

#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

bool Account::more_than(int m) {
  if (m >= 0 && m + money >= 0) {
    return true;
  }
  if (m <= 0 && m + money >= 0) {
    return true;
  }
  return false;
}

// Initialize bank
Bank::Bank(int p) {
  server = new Server(p);

  LoadPrivateKey("bank/bank-private.key", bankkey);

  AutoSeededRandomPool rnd;
  if (!bankkey.Validate(rnd, 3)) {
    cout << "Could not validate key" << endl;
  }
  
  buffer = new char[MSG_LEN];
  message = new char[MSG_LEN];
  
  database["Alice"] = new Account(100);
  database["Bob"] = new Account(50);
  database["Eve"] = new Account(0);
}

Bank::~Bank() {
  delete server;

  delete[] buffer;
  delete[] message;
  map<string, Account*>::iterator itr;

  for (itr = database.begin(); itr != database.end(); ++itr) {
    delete itr->second;
  }
}

void Bank::run() {
  socklen_t clilen;
  struct sockaddr_in addr;
  int i;
  
  fd_set read_connections;
  fd_set* connections = server->get_connections();
  while (1) {
    // Block until input arrives on sockets
    read_connections = *connections;
    if (select(FD_SETSIZE, &read_connections, NULL, NULL, NULL) < 0) {
      perror("Could not select connection");
      return;
    }

    // Service all sockets with input pending
    for (i=0; i<FD_SETSIZE; ++i) {
      if (FD_ISSET(i, &read_connections)) {
        // Connection request on server socket
        if (i == server->get_sockfd()) {
          int clisockfd;
          clilen = sizeof(addr);
          clisockfd = accept(server->get_sockfd(), (struct sockaddr*) &addr, &clilen);
          if (clisockfd < 0) {
            perror("Could not accept");
            return;
          }
          FD_SET(clisockfd, connections);
          new_atm(clisockfd);
        }
        // Data arriving on already connected socket
        else {
          if (read(i) < 0) {
            close(i);
            FD_CLR(i, connections);
            remove_atm(i);
          }
        }
      }
    }
  }
}

void Bank::new_atm(int fd) {
  atmkeyring[fd] = 0;
}

void Bank::remove_atm(int fd) {
  delete atmkeyring[fd];
  atmkeyring.erase(fd);
}

Session* Bank::find_atm_session(int fd) {
  map<int, Session*>::iterator itr = atmkeyring.find(fd);
  if (itr == atmkeyring.end()) {
    return 0;
  }
  return itr->second;
}

bool Bank::authenticate_atm(int fd, char* from, char* to) {
  RSA::PublicKey atmpub;
  load_key_from_buffer(atmpub, from);

  AutoSeededRandomPool rnd;
  if (!atmpub.Validate(rnd, 3)) {
    //cout << "Could not validate atm public key" << endl;
    return false;
  }

  Session* s = new Session;
  s->pub = atmpub;
  s->time = get_current_time();

  generate_aes_key(s->key, s->iv);

  atmkeyring[fd] = s;

  memset(to, 0, MSG_LEN);
  int offset = 0;
  memcpy(&to[offset], s->key, AES::DEFAULT_KEYLENGTH);
  offset += AES::DEFAULT_KEYLENGTH;
  memcpy(&to[offset], s->iv, AES::BLOCKSIZE);
  offset += AES::BLOCKSIZE;
  memcpy(&to[offset], &valid_message, 16);
  offset += 16;
  
  return true;
}

// Find account object associated with user,
// Return 0 if user does not exist
Account* Bank::find_account(string name) {
  map<string, Account*>::iterator itr = database.find(name);
  if (itr == database.end()) {
    cout << "Invalid user" << endl;
    return 0;
  }
  return itr->second;
}

bool Bank::authenticate_user(Session* session, char* from, char* to) {
  RSA_decrypt(from, to, bankkey);
  memcpy(from, to, MSG_LEN);
  RSA::PublicKey userpub;
  load_key_from_buffer(userpub, from);
  
  AutoSeededRandomPool rnd;
  if (!userpub.Validate(rnd, 3)) {
    //cout << "Could not validate user public key" << endl;
    return false;
  }

  Account* acc = find_account(session->auth);
  if (!acc) {
    //cout << "Could not find user" << endl;
    return false;
  }

  generate_aes_key(acc->key, acc->iv);

  memset(from, 0, MSG_LEN);
  int offset = 0;
  memcpy(&from[offset], acc->key, AES::DEFAULT_KEYLENGTH);
  offset += AES::DEFAULT_KEYLENGTH;
  memcpy(&from[offset], acc->iv, AES::BLOCKSIZE);
  offset += AES::BLOCKSIZE;
  memcpy(&from[offset], &valid_message, 16);
  offset += 16;
 
  RSA_encrypt(from, to, userpub);

  session->auth = "";
  
  return true;
}

int Bank::read(int fd) {
  recv_message(fd, buffer);
  // Check if atm is authenticated
  Session* session = find_atm_session(fd);
  if (session) {
    AES_decrypt(buffer, MSG_LEN, session->key, session->iv);

    if (session->auth != "") {
      authenticate_user(session, buffer, message);
    }
    else {
      read_header(session, buffer, message);
    }
    AES_encrypt(message, MSG_LEN, session->key, session->iv);
   }
  else {
    RSA_decrypt(buffer, message, bankkey);
    if (authenticate_atm(fd, message, buffer)) {
      session = find_atm_session(fd);
    }
    else {
      memset(buffer, 0, MSG_LEN);
      send_message(fd, buffer);
      return -1;
    }
    RSA_encrypt(buffer, message, session->pub);
  }
  return send_message(fd, message);
}

void Bank::read_header(Session* session, char* from, char* to) {
  int offset = 0;
  unsigned int strlen;
  bool auth;
  memcpy(&auth, &from[offset], sizeof(auth));
  offset += sizeof(auth);
  memcpy(&strlen, &from[offset], sizeof(strlen));
  offset += sizeof(strlen);
  if (strlen > MAX_STR) {
    create_message(-1, to);
    return;
  }
  char* str = new char[strlen];
  memcpy(str, &from[offset], strlen);
  string name(str, strlen);
  offset += strlen;
  
  if (auth) {
    session->auth = name;
    create_message(0, to);
  }
  else {
    Account* acc = find_account(name);
    if (!acc) {
      //cout << "Could not find user" << endl;
      create_message(-1, to);
      return;
    }
    
    AES_decrypt(&from[offset], MSG_LEN - offset, acc->key, acc->iv);    
    read_body(session, offset, from, to);
    AES_encrypt(to, MSG_LEN - offset, acc->key, acc->iv);
  }
}

void Bank::read_body(Session* session, int offset, char* from, char* to) {
  long long time;
  int amount;
  memcpy(&time, &from[offset], sizeof(time));
  offset += sizeof(time);
  memcpy(&amount, &from[offset], sizeof(amount));
  offset += sizeof(amount);
  unsigned int strlen;
  memcpy(&strlen, &from[offset], sizeof(strlen));
  offset += sizeof(strlen);
  if (strlen > MAX_STR) {
    create_message(-1, to);
    return;
  }
  char* str = new char[strlen];
  memcpy(str, &from[offset], strlen);
  string name(str, strlen);
  offset += sizeof(name);

  if (!valid_timestamp(session, time)) {
    //cout << "Invalid time" << endl;
    create_message(-1, to);
    return;
  }
  
  int response = transaction(name, amount);
  create_message(response, to);
}

bool Bank::valid_timestamp(Session* s, long long t) {
  if (s) {
    if (t > s->time) {
      s->time = t;
      return true;
    }
  }
  return false;
}

void Bank::create_message(int response, char* buf) {
  long long time = get_current_time();
  int offset = 0;
  memcpy(&buf[offset], &time, sizeof(time));
  offset += sizeof(time);
  memcpy(&buf[offset], &response, sizeof(response));
  offset += sizeof(response);
  pad(offset, buf);
}


int Bank::transaction(string name, int amount) {
  Account* acc = find_account(name);
  if (acc) {
    if (amount == 0) {
      return acc->get_money();
    }
    else if (acc->more_than(amount)) {
      return acc->add_money(amount);
    }
  }
  return -1;
}

bool Bank::send_message(int fd, char* buf) {
  int bytes = server->writen(fd, buf, MSG_LEN);
  if (bytes != MSG_LEN) {
    cout << "Bad write" << endl;
    return false;
  }
  return true;
}

bool Bank::recv_message(int fd, char* buf) {
  memset(buf, 0, MSG_LEN);
  int bytes = server->readn(fd, buf, MSG_LEN);
  if (bytes != MSG_LEN) {
    cout << "Bad read" << endl;
    return false;
  }
  return true;
}
