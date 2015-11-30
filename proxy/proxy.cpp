/*
  proxy.cpp - Proxy
*/
#include "proxy.h"

Proxy::Proxy(int ip, int op) {
  in = new Server(ip);
  out = new Socket(op);
}

void Proxy::run() {
  int bytes = 0;
  int cli = in->wait_for_connection();
  char *buffer = (char *)malloc(MSG_LEN * sizeof(char));
  bzero(buffer, MSG_LEN);
  while(1) {
    bytes = in->readn(cli, buffer, MSG_LEN);
    if (bytes != MSG_LEN) {
      cout << "Bad read" << endl;
      continue;
    }
    bytes = out->writen(buffer, MSG_LEN);
    if (bytes != MSG_LEN) {
      cout << "Bad write" << endl;
      continue;
    }
    bytes = out->readn(buffer, MSG_LEN);
    if (bytes != MSG_LEN) {
      cout << "Bad read" << endl;
      continue;
    }
    bytes = in->writen(cli, buffer, MSG_LEN);
    if (bytes != MSG_LEN) {
      cout << "Bad write" << endl;
      continue;
    }
  }
}
