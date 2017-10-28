.code32

.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

#Multiboot header
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
#Reserve stack (must be aligned to to 16 bytes due to compiler assumptions)
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .data
sorry64bitonly:
    .ascii "Here's a nickel kid. Get yourself a better computer. (Your processor seems to be 32-bit. BreaDOS only supports 64-bit processors.)"
sorry64bitonly_end:
.set sorry64bitonly_len, sorry64bitonly_end - sorry64bitonly

.section .text
.global _start
.type _start, @function
_start:
    movl $stack_top, %esp

    #Check for the existence of CPUID
    pushfl
    movl (%esp), %eax
    xorl $0x00200000, (%esp)
    popfl
    pushfl
    popl %ecx
    xorl %eax, %ecx
    je fail_long_mode

    #Check extended functions
    movl $0x80000000, %eax
    cpuid
    cmpl $0x80000001, %eax
    jb fail_long_mode

    #Check for long mode
    movl $0x80000001, %eax
    cpuid
    testl $0x20000000, %edx
    jz fail_long_mode

    call kernel_main
halt_and_catch_fire:
    cli
1:  hlt
    jmp 1b

#Handles 32-bit processors "gracefully" ;)
fail_long_mode:
    movl $sorry64bitonly_len, %ecx
    movl $sorry64bitonly, %esi
    call print_error
    jmp halt_and_catch_fire

#Length in %ecx, string pointer in %esi. Clobbers: %eax, %edi
print_error:
    movl $0xB8000, %edi
    movb $0x0F, %ah
print_error_loop:
    decl %ecx
    movb (%esi,%ecx,1), %al
    movw %ax, (%edi,%ecx,2)
    jnz print_error_loop
    ret

.size _start, . - _start

