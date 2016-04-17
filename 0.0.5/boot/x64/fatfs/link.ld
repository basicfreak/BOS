OUTPUT_FORMAT("binary")
ENTRY(Entry)
phys = 0x0000A000;
SECTIONS
{
  AP_Strap = 0x8800;

  .text phys : AT(phys) {
    code = .;
    *(.text)
    . = ALIGN(0x100);
  }
  .data : AT(phys + (data - code)) {
    data = .;
    *(.data)
    *(.eh_frame)
    *(.bss)
    . = ALIGN(0x100);
  }
  .rodata : AT(phys + (rodata - code)) {
    rodata = .;
    *(.rodata)
    . = ALIGN(0x100);
  }
  end = .;
  /DISCARD/ : {
    *(.comment)
    *(.note.GNU-stack)
  }
}

