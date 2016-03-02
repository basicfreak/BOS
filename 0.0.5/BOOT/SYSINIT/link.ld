OUTPUT_FORMAT("binary")
ENTRY(start)
phys = 0x00008000;
SECTIONS
{
  BS_ReadFile = 0x7D00;
  FILE_IO = 0xA000;

  .text phys : AT(phys) {
    FILL(0x90)
    *(.text)
    . = ALIGN(64);
    *(.data)
    . = ALIGN(0x800);
  }
  .CPUSection 0x8800 : AT(0x8800) {
    FILL(0x90)
    *(.CPUSection)
    . = ALIGN(0x200);
  }
  .rodata 0x8A00 : AT (0x8A00) {
    FILL(0x90)
    *(.rodata)
    . = ALIGN(0x1000);
  }
  .GDTSection 0x9000 : AT(0x9000) {
    FILL(0x90)
    *(.GDTSection)
  }
  end = .;
}

