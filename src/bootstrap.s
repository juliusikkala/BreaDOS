.code32
.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)
.set P_RW, 0b000000000011
.set EFER, 0xC0000080
.set PAE, 1<<5
.set LME, 1<<8
.set LMA, 1<<10
.set PG, 1<<31
.set KERNEL_VMA, 0xFFFFFFFF80000000

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
.align 8
GDT64:
GDT64_NULL:
.quad 0
GDT64_CODE:
.word 0, 0
.byte 0, 0b10011000, 0b00100000, 0
GDT64_DATA:
.word 0, 0
.byte 0, 0b10010000, 0, 0
GDT64_GDTR:
.word . - GDT64 - 1
.quad GDT64

sorry64bitonly:
.ascii "Here's a nickel kid. Get yourself a better computer. (Your processor seems to be 32-bit. BreaDOS only supports 64-bit processors.)"
sorry64bitonly_end:
.set sorry64bitonly_len, sorry64bitonly_end - sorry64bitonly

.section .text
.global _start
.type _start, @function
_start:
    movl $stack_top, %esp
    #Save multiboot information structure address (with upper bits zeroed)
    pushl %ebx
    pushl $0

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

    #Set up paging

    #Set up PML4T
    movl $0x1000, %edi #0x1000 is the first 4k-aligned usable memory block
    movl %edi, %cr3

    #Clear PML4T
    xorl %eax, %eax
    movl $0x1000, %ecx #0x1000 is the size of PML4T here
    rep stosl

    #Load the first entries of PML4T, PDPT and PDT, fill PT
    #Identity map 0-2MiB map KERNEL_VMA to the same 0-2MiB
    #PML4T
    movl $0x1000, %edi
    movl $0x2000|P_RW, (%edi)
    movl $0x1FF8, %edi
    movl $0x2000|P_RW, (%edi)
    #PDPT
    movl $0x2000, %edi
    movl $0x3000|P_RW, (%edi)
    movl $0x2FF0, %edi
    movl $0x3000|P_RW, (%edi)
    #PDT
    movl $0x3000, %edi
    movl $0x4000|P_RW, (%edi)

    #PT
    movl $0x4000, %edi
    movl $P_RW, %ebx
    movl $512, %ecx

page_table_loop:
    movl %ebx, (%edi)
    addl $0x1000, %ebx
    addl $8, %edi
    decl %ecx
    jnz page_table_loop
    
    #Enable PAE
    movl %cr4, %eax
    orl $PAE, %eax
    movl %eax, %cr4

    #Interrupts must be disabled in long mode until IDTR is loaded with 64-bit
    #interrupt descriptors 
    cli 

    #Enable long mode
    movl $EFER, %ecx
    rdmsr
    orl $LME, %eax
    wrmsr

    #Enable paging and thus activate long mode
    movl %cr0, %eax
    orl $PG, %eax
    movl %eax, %cr0

    #Check LMA
    rdmsr
    testl $LMA, %eax
    jz fail_long_mode

    #We are now successfully in the 32-bit compatibility submode of long mode.

    #Load 64-bit GDTR, enters the 64-bit submode.
    lgdt GDT64_GDTR
    ljmp $GDT64_CODE-GDT64, $enter64

#Handles 32-bit processors "gracefully" ;)
fail_long_mode:
    movl $sorry64bitonly_len, %ecx
    movl $sorry64bitonly, %esi
    call print_error
    #Halt and catch fire
    cli
1:  hlt
    jmp 1b

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

.code64
.global enter64
enter64:
    # Fix rsp address for virtual memory
    addq $KERNEL_VMA, %rsp

    # Restore multiboot information structure address
    popq %rdi
    addq $KERNEL_VMA, %rdi

    movabsq $kernel_main, %rax
    jmp *%rax

.size _start, . - _start

