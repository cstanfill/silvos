#ifndef __SILVOS_MEMORY_MAP_H
#define __SILVOS_MEMORY_MAP_H


/* First 1G page is kernel-access only. */

/* 0x000000000000 - 0x0000001FFFFF
 *
 * First 2M page has a single page directory, used by all threads.  All
 * kernel data structures must be mapped here.
 */

/* Null page:          0x000000000000  -  0x000000000FFF */

#define LOC_IDT        0x0000000F0000  /* 0x0000000F0FFF  (1 page)   */
#define LOC_FP_BUF     0x0000000FA000  /* 0x0000000FAFFF  (1 page)   */
#define LOC_IST1_STACK 0x0000000FB000  /* 0x0000000FBFFF  (1 page)   */
#define LOC_TEMP_PT    0x0000000FC000  /* 0x0000000FCFFF  (1 page)   */
#define LOC_TEMP_PT2   0x0000000FD000  /* 0x0000000FDFFF  (1 page)   */
#define LOC_TEMP_PT3   0x0000000FE000  /* 0x0000000FEFFF  (1 page)   */
#define LOC_IDLE_STACK 0x0000000FF000  /* 0x0000000FFFFF  (1 page)   */

/* Kernel text:        0x000000100000  -  0x0000001FFFFF */

/* 0x000000200000 - 0x0000003FFFFF
 *
 * Second 2M page is for thread-local storage.
 */

#define LOC_THREAD_FP   0x000000200000  /* 0x000000200FFF  (1 page)   */
#define LOC_KERN_STACK  0x0000002FF000  /* 0x0000002FFFFF  (1 page)   */

/* Rest of first 1G page is unused */

#define LOC_USERZONE_BOT 0x000040000000

/* 0x000040000000 - 0x7FFFFFFFFFFF
 *
 * Lower half of memory: userland memory
 */

#define LOC_TEXT        0x000040000000
#define LOC_USER_STACK  0x0000401FF000

#define LOC_USERZONE_TOP 0x800000000000

/* 0x800000000000 - 0xFF7FFFFFFFFF
 *
 * Upper half of memory: currently unused.
 */


/* 0xFF8000000000 - 0xFFFFFFFFFFFF
 *
 * Last 512G of memory map to the pagetables.  See page.h for how to find
 * various levels of page table for various locations.
 */


#endif
