#include "userland.h"
#include "userland-lib.h"

int depth = 0;

static const long long NEW_STACK = 0x10000000;

static int bluh = 2;

void __attribute__ ((noreturn)) my_task() {
  debug("I'M ALIVE");
  if (bluh == 5) {
    debug("Yeah, he did it.");
  }
  exit();
}

void main() {
  long long stac = NEW_STACK;
  for (int i = 0; i < 8; i++) {
    if (palloc((const void *)stac)) {
      debug("ALLOC FAIL");
    }
    int child_id = spawn_thread(my_task, (void *)(stac + 0x1000));
    if (child_id < 0) {
      debug("SPAWN FAIL");
    }
    debug_printf("Created child 0x%02X", child_id);
    stac += 0x4000;
  }
  bluh = 5;
  debug("I didn't die, and...");
}
