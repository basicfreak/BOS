OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
ENTRY(start)
phys = 0x00000000;
SECTIONS
{
  .text phys : AT(phys) {
    code = .;
    *(.text)
    *(.rodata)
    . = ALIGN(0x1000);
  }
  .data : AT(phys + (data - code))
  {
    data = .;
    *(.data)
    . = ALIGN(0x1000);
  }
  .bss : AT(phys + (bss - code))
  {
    bss = .;
    *(.bss)
    . = ALIGN(0x1000);
  }
  end = .;
}