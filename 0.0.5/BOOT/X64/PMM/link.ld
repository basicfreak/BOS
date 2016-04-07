OUTPUT_FORMAT("binary")
ENTRY(_init)
phys = 0xFFFFFFFF80110000;
SECTIONS
{
  FILE_IO = 0xA000;
  AP_Strap = 0x8800;
  . = phys;
  .init phys : AT(phys) {
    FILL(0x90)
    init = .;
    *(.init)
    . = ALIGN(0x1000);
  }
  .text : AT(phys + (text - init)) {
    FILL(0x90)
    text = .;
    *(.text)
    . = ALIGN(0x1000);
  }
  .data : AT(phys + (data - init)) {
    FILL(0x90)
    data = .;
    *(.data)
    . = ALIGN(0x100);
    *(.bss)
    . = ALIGN(0x1000);
  }
  .rodata : AT(phys + (rodata - init)) {
    FILL(0x90)
    rodata = .;
    *(.rodata)
    . = ALIGN(0x1000);
  }
  /DISCARD/ : {
    *(.comment)
    *(.note.GNU-stack)
    *(.eh_frame)
  }
  end = .;
}

