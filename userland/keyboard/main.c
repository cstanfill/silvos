#include "userland.h"
#include "userland-lib.h"

#define BUFSIZE 2
#define CIRC_CNT(head,tail,size) (((head) - (tail)) % (size))
#define CIRC_SPACE(head,tail,size) CIRC_CNT((tail),((head)+1),(size))
#define assert(x) if (!(x)) { debug_printf("assert at %s:%d failed: %s", __FILE__, __LINE__, #x); exit(); }

ipc_msg respond2(ipc_msg send) {
  sendrecv_op op = { .send = send };
  assert(respond(&op) == MESSAGE_RECEIVED);
  return op.recv;
}

void service(uint64_t worker_pid) {
  char inbox[BUFSIZE];
  size_t in_head = 0;
  size_t in_tail = 0;
  memset(inbox, '\0', sizeof(inbox));

  typedef struct { uint64_t thread_id; semaphore_id sem; } wait;
  wait clients[BUFSIZE];
  size_t wait_head = 0;
  size_t wait_tail = 0;
  memset(clients, '\0', sizeof(clients));

  typedef struct { uint64_t thread_id; char c; } out;
  out outbox[BUFSIZE];
  size_t out_ix = 0;
  memset(outbox, '\0', sizeof(outbox));

  ipc_msg _response = { .addr = 0 };
  ipc_msg* const response = &_response;
  while (1) {
    const ipc_msg request = respond2(*response);
    response->addr = request.addr;
    response->r1 = request.r1;
    response->r2 = 0;
    if (request.addr == worker_pid) {
      /* Insert a character into the inbox if nobody's waiting;
       * else pop a waiting client and move them to the outbox. */
      char c = (char)(uint8_t)request.r1;
      if (CIRC_CNT(wait_head, wait_tail, BUFSIZE) > 0) {
        sem_set(clients[wait_tail].sem);
        outbox[out_ix].thread_id = clients[wait_tail].thread_id;
        outbox[out_ix].c = c;
        wait_tail = (wait_tail + 1) % BUFSIZE;
        out_ix = (out_ix + 1) % BUFSIZE;
      } else {
        inbox[in_head] = c;
        if (CIRC_SPACE(in_head, in_tail, BUFSIZE) == 0) {
          /* Buffer overfull; drop oldest inbox entry */
          in_tail = (in_tail + 1) % BUFSIZE;
        }
        in_head = (in_head + 1) % BUFSIZE;
      }
    } else if (request.r1 == 0) {
      /* Requesting a character, semaphore is in r2 */
      if (CIRC_CNT(in_head, in_tail, BUFSIZE) > 0) {
        /* Already have a ready character. */
        response->r1 = 0;
        response->r2 = inbox[in_tail];
        in_tail = (in_tail + 1) % BUFSIZE;
      } else if (CIRC_SPACE(wait_head, wait_tail, BUFSIZE) == 0) {
        /* Too many waiting clients, fail. */
        response->r1 = -1;
      } else {
        clients[wait_head].thread_id = request.addr;
        clients[wait_head].sem = request.r2;
        response->r1 = 1;
        response->r2 = (out_ix + CIRC_CNT(wait_head, wait_tail, BUFSIZE)) % BUFSIZE;
        wait_head = (wait_head + 1) % BUFSIZE;
      }
    } else if (request.r1 == 1) {
      /* Requesting a character, index into `outbox` is in r2 */
      uint64_t ix = request.r2;
      if (ix >= BUFSIZE) {
        response->r1 = -1;
      } else if (outbox[ix].thread_id != request.addr) {
        response->r1 = -1;
      } else {
        response->r1 = 0;
        response->r2 = outbox[ix].c;
        outbox[ix].thread_id = 0;
      }
    } else {
      /* Unknown message */
      response->r2 = -1;
    }
  }
}

void worker (uint64_t service_tid) {
  sendrecv_op op;
  while (1) {
    char c = _getch();
    op.send.addr = service_tid;
    op.send.r1 = c;
    op.send.r2 = 0;
    if (call(&op)) {
      debug("CALL FAILED IN WORKER");
      return;
    }
  }
}

void main() {
  /* Child */
  uint64_t listener_pid = get_tid();
  uint64_t fret = fork();
  if (fret == 0) {
    /* Worker child */
    worker(listener_pid);
  } else {
    /* Listener child */
    uint64_t worker_pid = fret;
    service(worker_pid);
  }
}
