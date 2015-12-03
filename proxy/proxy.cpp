/*
  proxy.cpp - Proxy
*/
#include "proxy.h"

// Initialize proxy
Proxy::Proxy(int ip, int op) {
  in = new Server(ip);
  out = new Socket(op);

  buffer = new char[MSG_LEN];
}

Proxy::~Proxy() {
  delete out;
  delete in;

  delete[] buffer;
}

// Loop to read from in and write to out, then
// read from out and write to in for bidirectional traffic
void Proxy::run() {
  cout << "Started" << endl;
  socklen_t clilen;
  struct sockaddr_in addr;
  int i;
  
  fd_set read_connections;
  fd_set connections = *in->get_connections();
  while (1) {
    // Block until input arrives on sockets
    read_connections = connections;
    if (select(FD_SETSIZE, &read_connections, NULL, NULL, NULL) < 0) {
      perror("Could not select connection");
      return;
    }

    // Service all sockets with input pending
    for (i=0; i<FD_SETSIZE; ++i) {
      if (FD_ISSET(i, &read_connections)) {
        // Connection request on server socket
        if (i == in->get_sockfd()) {
          int clisockfd;
          clilen = sizeof(addr);
          clisockfd = accept(in->get_sockfd(), (struct sockaddr*) &addr, &clilen);
          if (clisockfd < 0) {
            perror("Could not accept");
            return;
          }
          FD_SET(clisockfd, &connections);
        }
        // Data arriving on already connected socket
        else {
          if (handle_message(i) < 0) {
            close(i);
            FD_CLR(i, &connections);
          }
        }
      }
    }
  }
}

int Proxy::handle_message(int cli) {
  memset(buffer, 0, MSG_LEN);
  int bytes = in->readn(cli, buffer, MSG_LEN);
  if (bytes < 0) {
    cout << "Bad client read" << endl;
    return -1;
  }
  bytes = out->writen(buffer, MSG_LEN);
  if (bytes < 0) {
    cout << "Bad server write" << endl;
    return -1;
  }
  memset(buffer, 0, MSG_LEN);
  bytes = out->readn(buffer, MSG_LEN);
  if (bytes < 0) {
    cout << "Bad server read" << endl;
    return -1;
  }
  bytes = in->writen(cli, buffer, MSG_LEN);
  if (bytes < 0) {
    cout << "Bad client write" << endl;
    return -1;
  }
  return bytes;
}
