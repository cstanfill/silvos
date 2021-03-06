#ifndef __SILVOS_IDT_H
#define __SILVOS_IDT_H

void create_idt (void);
void register_isr (unsigned char num,
                   void (*handler)(void),
                   unsigned char ist);
void register_user_isr (unsigned char num,
                        void (*handler)(void),
                        unsigned char ist);

#endif
