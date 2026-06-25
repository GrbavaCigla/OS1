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
