#include "userland.h"
#define DEBUG(str) debug(str, sizeof(str))
/* Base of zone to allocate at */
#define MEMORY_BASE (unsigned long)(0x10000000)
#define PAGE_SIZE 0x1000

void main() {
  long x = 0;
  while (1) {
    for (int j = 0; j < 200; ++j) {
      char *page = (char *)(MEMORY_BASE + j * PAGE_SIZE);
      if (palloc(page)) {
        DEBUG("ALLOC FAIL");
        exit();
      }
    }
    for (int j = 0; j < 200; ++j) {
      char *page = (char *)(MEMORY_BASE + j * PAGE_SIZE);
      if (pfree(page)) {
        DEBUG("FREE FAIL");
        exit();
      }
    }
    ++x;
    if (x % 200 == 0) {
      DEBUG("Bing!");
    }
  }
}
