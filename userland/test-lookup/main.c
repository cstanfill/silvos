#include "userland.h"

#define DEBUG(str) debug(str, sizeof(str))

void main (void) {
  fork();
  if (find("test-lookup.bin") != 1) {
    DEBUG("FOUND WRONG THREAD");
  }
}
