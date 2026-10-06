#include "patch.h"

#include <stdint.h>

/*
((void (*)(void))0x800d61a0)();
((void (*)(void*, uint32_t))0x80004300)(ptr, 0x1000);
*/

void uncomp_tramp(uint32_t src, void* dst, uint32_t len);

asm(
    ".text                 \n"
    ".set noreorder        \n"
    "                      \n"
    ".ent uncomp_tramp     \n"
    "uncomp_tramp:         \n"
    "    addi $sp, $sp, -64\n"
    "                      \n"
    "    sw $ra, 56($sp)   \n"
    "    sw $s0, 48($sp)   \n"
    "    sw $s1, 40($sp)   \n"
    "    sw $s2, 32($sp)   \n"
    "                      \n"
    "    move $s0, $a0     \n"
    "    move $s1, $a1     \n"
    "    .word 0x0C000267  \n" // jal 0x8000099C
    "     move $s2, $a2    \n"
    "                      \n"
    "    move $a0, $s0     \n"
    "    move $a1, $s1     \n"
    "    bal uncomp_hook   \n"
    "     move $a2, $s2    \n"
    "                      \n"
    "    lw $s2, 32($sp)   \n"
    "    lw $s1, 40($sp)   \n"
    "    lw $s0, 48($sp)   \n"
    "    lw $ra, 56($sp)   \n"
    "                      \n"
    "    jr $ra            \n"
    "     addi $sp, $sp, 64\n"
    ".end uncomp_tramp     \n"
    "                      \n"
    ".set reorder          \n"
);

void uncomp_hook(uint32_t src, void* dst, uint32_t len) {

}

void comp_tramp(uint32_t src, void* dst, uint32_t len);

asm(
    ".text                 \n"
    ".set noreorder        \n"
    "                      \n"
    ".ent comp_tramp       \n"
    "comp_tramp:           \n"
    "    addi $sp, $sp, -64\n"
    "                      \n"
    "    sw $ra, 56($sp)   \n"
    "    sw $s0, 48($sp)   \n"
    "    sw $s1, 40($sp)   \n"
    "    sw $s2, 32($sp)   \n"
    "                      \n"
    "    move $s0, $a0     \n"
    "    move $s1, $a1     \n"
    "    .word 0x0C0004E5  \n" // jal 0x80001394
    "     move $s2, $a2    \n"
    "                      \n"
    "    move $a0, $s0     \n"
    "    move $a1, $s1     \n"
    "    bal comp_hook     \n"
    "     move $a2, $s2    \n"
    "                      \n"
    "    lw $s2, 32($sp)   \n"
    "    lw $s1, 40($sp)   \n"
    "    lw $s0, 48($sp)   \n"
    "    lw $ra, 56($sp)   \n"
    "                      \n"
    "    jr $ra            \n"
    "     addi $sp, $sp, 64\n"
    ".end comp_tramp       \n"
    "                      \n"
    ".set reorder          \n"
);

void comp_hook(uint32_t src, void* dst, uint32_t len) {
    // code
    if (src == 0x00A58A00) {
        // white tunic
        *(uint32_t*)(dst + 0xE4398) = 0xFFFFFF64;
    }

    // ovl_title
    if (src == 0x00AFC8E0) {
        // disable logo spin (nop the instruction that increases the angle)
        *(uint32_t*)(dst + 0x660) = 0x00000000;
        ((void (*)(void))0x800d61a0)(); // osWritebackDCacheAll
    }
}

void patch_main(void* entrypoint, void* this) {
    volatile uint32_t* ptr = entrypoint;

    // ntsc video
    ptr[0x2A4 / sizeof(uint32_t)] = 0x240F0002;
    ptr[0x2AC / sizeof(uint32_t)] = 0x27396950;
    ptr[0x2EC / sizeof(uint32_t)] = 0x3C013F80;
    ptr[0x2F0 / sizeof(uint32_t)] = 0x44812000;
    ptr[0x2F4 / sizeof(uint32_t)] = 0xAF090000;
    ptr[0x2F8 / sizeof(uint32_t)] = 0xAF080004;

    // disable rspboot antipiracy
    ptr[0x61B0 / sizeof(uint32_t)] = 0x201010C8;
    ptr[0x6240 / sizeof(uint32_t)] = 0x10000007;

    // don't clear RAM
    ptr[0xE8 / sizeof(uint32_t)] = 0x00000000;

    // inject decompressed file hook (jal uncomp_tramp)
    ptr[0x8EC / sizeof(uint32_t)] = 0x0C000000 | ((RELOCATE(uncomp_tramp, this) & 0x0FFFFFFF) >> 2);

    // inject compressed file hook (jal comp_tramp)
    ptr[0x960 / sizeof(uint32_t)] = 0x0C000000 | ((RELOCATE(comp_tramp, this) & 0x0FFFFFFF) >> 2);
}

DECLARE_PATCH(
    0xB2055FBD'0BAB4E0C, // CRC of OoT PAL 1.1
    patch_main
)
