OUTPUT_FORMAT("binary")
ENTRY(start)
phys = 0x00008000;
SECTIONS
{
  BS_ReadFile = 0x7D00;
  FILE_IO = 0xA000;

  .text phys : AT(phys) {
    *(.text)
    . = ALIGN(0x10);
    *(.data)
  }
  .CPUSection 0x8800 : AT(0x8800) {
    *(.CPUSection)
  }
  .rodata 0x8A00 : AT (0x8A00) {
    *(.rodata)
  }
  .GDTSection 0x9000 : AT(0x9000) {
    *(.GDTSection)
  }
  end = .;
}

