.extern handleSupervisorTrap
.global supervisorTrap
.align 4
supervisorTrap:
    addi sp, sp, -256
    .irp index, 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    call handleSupervisorTrap

    csrr t0, scause
    li t1, 9
    beq t0, t1, skip
    li t1, 8
    beq t0, t1, skip

    ld x10, 80(sp)
skip:
    .irp index, 1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    
    addi sp, sp, 256
    sret