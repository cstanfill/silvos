#include "userland.h"
#include "userland-lib.h"

void main (void) {
  while(1) {
    putch(getch());
  }
}
