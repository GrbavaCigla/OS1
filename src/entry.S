# Bare-metal entry point. Overrides the _entry in hw.lib's entry.o, which uses
# a GOT relocation (R_RISCV_GOT_HI20) for `stack0`. In a static bare-metal link
# that crashes binutils 2.45 (assertion fail in elfnn-riscv.c). This version is
# identical except it uses PC-relative addressing (la -> auipc+addi), so the
# linker pulls ours instead of the broken archive member.
.section .entry_os
.global _entry
_entry:
    # set up a per-hart stack: sp = stack0 + (4096 * (mhartid + 1))
    la sp, stack0
    li a0, 1024*4
    csrr a1, mhartid
    addi a1, a1, 1
    mul a0, a0, a1
    add sp, sp, a0
    call start
spin:
    j spin
