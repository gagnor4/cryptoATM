#ifndef PROXY_H
#define PROXY_H

#include "../lib/util.h"

#include <sys/ioctl.h>

class Proxy {
public:
  Proxy(int ip, int op);
  ~Proxy();
  void run();
private:
  int handle_message(int fd);
  
  Server* in;
  Socket* out;
  char* buffer;
};

#endif
