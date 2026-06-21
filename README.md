# OS1

## Fedora 43 build fixes

The provided Makefile and `lib/hw.lib` require the following changes to build on Fedora 43 (`binutils-2.45-1.fc43`, `riscv64-linux-gnu-*` toolchain).

### 1. `src/entry.S` (new file)
`lib/hw.lib`'s `entry.o` was compiled with GOT-style addressing (`R_RISCV_GOT_HI20`) for the `stack0` symbol. In a static bare-metal link, `riscv64-linux-gnu-ld` 2.45 hits an assertion in `elfnn-riscv.c` and segfaults instead of resolving it. A drop-in replacement written in `src/entry.S` uses PC-relative addressing (`la` → `auipc`+`addi`) for the same logic. Because it is a real object file, the linker uses it and never pulls the broken archive member.

### 2. `ASFLAGS`: remove `-ggdb`
The Makefile's `ASFLAGS` contained `-ggdb`, a GCC-only debug flag. The GNU assembler (`as`) does not accept it and fails with `Fatal error: unknown option '-ggdb'`. Removed.

### 4. `-march=rv64ima_zicsr`
`trap.S` uses `csrr` (CSR read), which belongs to the `zicsr` extension. Without the explicit sub-extension in `-march`, the assembler rejects the instruction. Added `_zicsr` to all `-march=rv64ima` flags in the Makefile.

## Reminders
- Check permissions