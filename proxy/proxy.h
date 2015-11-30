#ifndef PROXY_H
#define PROXY_H

#include "../lib/util.h"

#include <stdlib.h>
#include <string.h>

class Proxy {
public:
  Proxy(int ip, int op);
  void run();
private:
  Server* in;
  Socket* out;
};

#endif
