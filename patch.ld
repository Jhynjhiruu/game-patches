OUTPUT_FORMAT ("elf32-bigmips")
OUTPUT_ARCH (mips)

MEMORY {
    CACHED : ORIGIN = 0x00000000, LENGTH = 0x00800000
    DUMMY : ORIGIN = 0x00000000, LENGTH = 0xFFFFFFFF
}

SECTIONS {
    __romPos = 0;

    .crc : AT(0xF0000000) {
        KEEP(*(.crc*));
    } > DUMMY

    .hdr : AT(__romPos) {
        KEEP(*(.hdr*))
        . = ALIGN(16);
    } > CACHED
    __romPos += SIZEOF(.hdr);
    __romPos = ALIGN(__romPos, 16);

    .text 0x00000000 : AT(__romPos) {
        *(.text*);
        . = ALIGN(16);
    } > CACHED
    __romPos += SIZEOF(.text);
    __romPos = ALIGN(__romPos, 16);

    /*.data ALIGN(16) : AT(__romPos) {
        *(.data*);
        . = ALIGN(16);
    } > CACHED
    __romPos += SIZEOF(.data);
    __romPos = ALIGN(__romPos, 16);*/

    osTvType            = 0x80000300;
    osRomType           = 0x80000304;
    osRomBase           = 0x80000308;
    osResetType         = 0x8000030C;
    osCicId             = 0x80000310;
    osVersion           = 0x80000314;
    osMemSize           = 0x80000318;
    osAppNMIBuffer      = 0x8000031C;

    /DISCARD/ : {
        *(.MIPS.*)
        *(.comment)
        *(.mdebug.*)
        *(.pdr)
        *(.reginfo)
        *(.eh_frame)

        /*
         * We may need the global offset table some day.
         * Our target is currently set with a static relocation-model, so this
         * might not be needed after all.
         */
        *(.got)
    }
}
