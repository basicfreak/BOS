OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
TARGET(elf32-i386)
Phys = 0x00000000;
SECTIONS
{
  .text Phys : AT(Phys)
  {
    code = .; _code = .; __code = .;
    *(.text)
    . = ALIGN(0);
  }

  .data : AT(Phys + (data - code))
  {
     data = .; _data = .; __data = .;
     *(.data)
     *(.rodata)
    . = ALIGN(0);
  }

  .bss : AT(Phys + (bss - code))
  {
    bss = .; _bss = .; __bss = .;
    *(.bss)
    . = ALIGN(0);
  }

  /DISCARD/ : {
      *(.comment)
    }

  end = .; _end = .; __end = .;
}