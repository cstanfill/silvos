
/* Hardware interrupts must push all caller-save registers.   Syscall handlers
 * don't need to do anything.  (Callee-save registers are saved later.  For
 * hardware interrupts, this means all registers are saved.  The syscall ABI
 * lets the kernel clobber caller-save registers.)  Be careful about using any
 * registers in these routines. */

.macro push_caller_save_reg
	push %rax
	push %rcx
	push %rdx
	push %rsi
	push %rdi
	push %r8
	push %r9
	push %r10
	push %r11
.endm

.macro pop_caller_save_reg
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rdi
	pop %rsi
	pop %rdx
	pop %rcx
	pop %rax
.endm

.macro clear_caller_saved_reg_except_rax
	xor %r11, %r11
	xor %r10, %r10
	xor %r9, %r9
	xor %r8, %r8
	xor %rdi, %rdi
	xor %rsi, %rsi
	xor %rdx, %rdx
	xor %rcx, %rcx
.endm

/* Syscalls */

/* Note: the syscall ABI is:
 *
 *  REG | INPUT | OUTPUT
 *  --------------------
 *  RAX | SYSNO | RETURN
 *  RBX | ARG1  | SAVED
 *  RCX | ARG2  | UNDEF
 *  RDX | ----- | UNDEF
 *  RBP | ----- | SAVED
 *  RSI | ----- | UNDEF
 *  RDI | ----- | UNDEF
 *  RSP | ----- | SAVED
 *  R8  | ----- | UNDEF
 *  R9  | ----- | UNDEF
 *  R10 | ----- | UNDEF
 *  R11 | ----- | UNDEF
 *  R12 | ----- | SAVED
 *  R13 | ----- | SAVED
 *  R14 | ----- | SAVED
 *  R15 | ----- | SAVED
 *
 * This matches the SYSV x86_64 ABI for caller/callee saved registers,
 * but not for arguments */

.GLOBAL syscall_isr
syscall_isr:
	movq (syscall_defns_len), %r8
	cmpq %r8, %rax
	jae invalid_syscall

	mov $syscall_defns, %r8
	mov (%r8, %rax, 8), %rax

	/* Translate syscall arguments into appropriate regs for handler */
	mov %rbx,%rdi
	mov %rcx,%rsi
	call *%rax

	/* Clear registers to prevent information leakage from kernel mode
	 * to usermode. For *callee* saved registers, it would be an ABI
	 * to not have preserved them, so we only need to clobber caller
	 * saved registers */
	clear_caller_saved_reg_except_rax
	iretq

invalid_syscall:
	mov $-1, %rax
	/* Note: caller saved registers don't need to be clobbered */
	iretq

/* Interrupts */

.GLOBAL kbd_isr
kbd_isr:
	push_caller_save_reg
	call master_eoi
	call read_key
	pop_caller_save_reg
	iretq

.GLOBAL timer_isr
timer_isr:
	push_caller_save_reg
	call master_eoi
	call yield
	pop_caller_save_reg
	iretq

.GLOBAL rtc_isr
rtc_isr:
	push_caller_save_reg
	call slave_eoi
	call hpet_sleepers_awake
	pop_caller_save_reg
	iretq

/* Faults */

.GLOBAL nm_isr
nm_isr:
	push_caller_save_reg
	call fpu_activate
	pop_caller_save_reg
	iretq

.GLOBAL fault_isr
fault_isr:
	push_caller_save_reg
	call thread_exit_fault

.GLOBAL df_isr
df_isr:
	hlt
	jmp df_isr

.GLOBAL pf_isr
pf_isr:
	push_caller_save_reg
	mov %cr2,%rdi
	call pagefault_handler
	pop_caller_save_reg
	iretq
end_pf_isr:


/* Unwind information for pf_isr */
    .section .eh_frame
    .align 8
cie_start:
    .long cie_end - cie_start - 0x04 /* length of CIE */
    .long 0x00000000  /* CIE 'magic' */
    .byte 0x01        /* CIE version */
    .byte 'z'         /* Augmentation data = zR\0 */
    .byte 'R'
    .byte 0x00
    .byte 0x01        /* Code alignment = 1 */
    .word 0x1078      /* Data alignment = -8 */
    .byte 0x01        /* Augmentation data length */
    .byte 0x1b        /* Pointer encoding: relative, signed 32 */
    .byte 0x0c        /* Start of initial instructions: Define CFA [...] */
    .word 0x0807      /* [...] CFA = r7 (rsp) + 8 */
    .word 0x0190      /* r16 (rip) = CFA + 1 * (-8) */
    .align 8
cie_end:
    .long end_eh_frame - . - 0x04
    .long . - cie_start /* CIE pointer */
    .reloc ., R_X86_64_PC32, .text + (pf_isr - .text)
    .long 0x00000000    /* filled by reloc */
    .long end_pf_isr - pf_isr
    .byte 0x00          /* Aug data */
    .word 0x200e        /* CFA = rsp+32 */
    .word 0x0390        /* rip @ CFA-24 */
    .byte 0x41          /* advance 1 */
    .word 0x280e        /* CFA = rsp+40 */
    .byte 0x41          /* [...] */
    .word 0x300e
    .byte 0x41
    .word 0x380e
    .byte 0x41
    .word 0x400e
    .byte 0x41
    .word 0x480e
    .byte 0x42          /* Some pushes are 2 bytes */
    .word 0x500e
    .byte 0x42
    .word 0x580e
    .byte 0x42
    .word 0x600e
    .byte 0x42
    .word 0x680e        /* CFA = rsp+104 */
    .byte 0x4a          /* advance 10 (past 'call') */
    .word 0x600e        /* CFA = rsp+96 */
    .byte 0x42          /* advance 2 */
    .word 0x580e        /* [...] */
    .byte 0x42
    .word 0x500e
    .byte 0x42
    .word 0x480e
    .byte 0x41
    .word 0x400e
    .byte 0x41
    .word 0x380e
    .byte 0x41
    .word 0x300e
    .byte 0x41
    .word 0x280e
    .byte 0x41
    .align 8
end_eh_frame:
