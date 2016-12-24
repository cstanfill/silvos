#include "page.h"

#include "memory-map.h"

#include "alloc.h"
#include "util.h"

#include <stdint.h>

pagetable kernel_pdpt;

void insert_pt (pagetable pt) {
  __asm__("mov %0,%%cr3" : : "r"((pagetable)virt_to_phys((uint64_t)pt)) : );
}


extern int _end;

pagetable initial_pt (void) {
  /* Set up the shared kernel pt */
  kernel_pdpt = (pagetable)allocate_phys_page();
  for (uint64_t j = 0x00; j < PAGE_PT_NUM_ENTRIES - 2; j++) {
    kernel_pdpt[j] = (j * PAGE_1G_SIZE) | PAGE_MASK__KERNEL | PAGE_MASK_SIZE;
  }
  kernel_pdpt[PAGE_PT_NUM_ENTRIES-2] = PAGE_MASK__KERNEL | PAGE_MASK_SIZE;
  kernel_pdpt[PAGE_PT_NUM_ENTRIES-1] = PAGE_1G_SIZE | PAGE_MASK__KERNEL | PAGE_MASK_SIZE;
  return new_pt();
}

/* Return a pagetable that contains enough information to set it as your
 * pagetable. */
pagetable new_pt (void) {
  pagetable pml4 = (pagetable)allocate_phys_page();
  memset(pml4, 0x00, PAGE_4K_SIZE);
  pml4[PAGE_PT_SELF_MAP] = virt_to_phys((uint64_t)pml4) | PAGE_MASK__KERNEL;
  pml4[PAGE_PT_NUM_ENTRIES-1] = virt_to_phys((uint64_t)kernel_pdpt) | PAGE_MASK__KERNEL;
  return pml4;
}

int map_page (uint64_t phys, uint64_t virt, uint64_t mode) {
  if (phys & PAGE_4K_MASK) {
    return -1;
  }
  if (virt & PAGE_4K_MASK) {
    return -1;
  }

  pagetable pml4 = (pagetable) PAGE_VIRT_PML4_OF(virt);
  pagetable pdpt = (pagetable) PAGE_VIRT_PDPT_OF(virt);
  pagetable pd = (pagetable) PAGE_VIRT_PD_OF(virt);
  pagetable pt = (pagetable) PAGE_VIRT_PT_OF(virt);

  if (!(pml4[PAGE_HT_OF(virt)] & PAGE_MASK_PRESENT)) {
    pml4[PAGE_HT_OF(virt)] = virt_to_phys((uint64_t)allocate_phys_page()) | PAGE_MASK__USER;
    memset(pdpt, 0x00, PAGE_4K_SIZE);
  }
  if (!(pdpt[PAGE_1G_OF(virt)] & PAGE_MASK_PRESENT)) {
    pdpt[PAGE_1G_OF(virt)] = virt_to_phys((uint64_t)allocate_phys_page()) | PAGE_MASK__USER;
    memset(pd, 0x00, PAGE_4K_SIZE);
  }
  if (!(pd[PAGE_2M_OF(virt)] & PAGE_MASK_PRESENT)) {
    pd[PAGE_2M_OF(virt)] = virt_to_phys((uint64_t)allocate_phys_page()) | PAGE_MASK__USER;
    memset(pt, 0x00, PAGE_4K_SIZE);
  }

  pt[PAGE_4K_OF(virt)] = phys | mode;
  return 0;
}

int unmap_page (uint64_t virt) {
  if (virt & PAGE_4K_MASK) {
    return -1;
  }

  pagetable pml4 = (pagetable) PAGE_VIRT_PML4_OF(virt);
  pagetable pdpt = (pagetable) PAGE_VIRT_PDPT_OF(virt);
  pagetable pd = (pagetable) PAGE_VIRT_PD_OF(virt);
  pagetable pt = (pagetable) PAGE_VIRT_PT_OF(virt);

  if (!(pml4[PAGE_HT_OF(virt)] & PAGE_MASK_PRESENT)) {
    return 0;
  }
  if (!(pdpt[PAGE_1G_OF(virt)] & PAGE_MASK_PRESENT)) {
    return 0;
  }
  if (!(pd[PAGE_2M_OF(virt)] & PAGE_MASK_PRESENT)) {
    return 0;
  }

  pt[PAGE_4K_OF(virt)] = PAGE_MASK__FAKE;
  return 0;
}

int map_new_page (uint64_t virt, uint64_t mode) {
  return map_page(virt_to_phys((uint64_t)allocate_phys_page()), virt, mode);
}
