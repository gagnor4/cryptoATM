/*
  main.cpp - Proxy
*/
#include "proxy.h"
#include "../lib/util.h"

#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "Wrong number of arguments\n";
    cout << "Usage: ./proxy inport outport" << endl;
    return 1;
  }
  int iport = atoi(argv[1]);
  int oport = atoi(argv[2]);

  if (iport < 1024 || iport > 65000 ||
      oport < 1024 || oport > 65000) {
    cout << "Invalid port" << endl;
    return 1;
  }

  Proxy proxy(iport, oport);

  proxy.run();
  
  return 0;
}
