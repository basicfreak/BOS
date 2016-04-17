OUTPUT_FORMAT("binary")
ENTRY(start)
OUTPUT_ARCH(i386)
phys = 0x00100000;
SECTIONS
{
  FILE_IO = 0xA000;
  AP_Strap = 0x8800;
  .text phys : AT(phys) {
    text = .;
    *(.text)
    . = ALIGN(0x1000);
  }
  .data : AT(phys + (data - text)) {
    data = .;
    *(.data)
    *(.eh_frame)
    *(.bss)
    . = ALIGN(0x1000);
  }
  .rodata : AT(phys + (rodata - text)) {
    rodata = .;
    *(.rodata)
    . = ALIGN(0x1000);
  }
  /DISCARD/ : {
    *(.comment)
    *(.note.GNU-stack)
  }
  end = .;
}

