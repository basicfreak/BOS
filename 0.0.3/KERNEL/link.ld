OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
TARGET(elf32-i386)
ENTRY(start)
SECTIONS
{
  . = 0xC0100000;
  .text : AT(ADDR(.text) - 0xC0000000)
  {
    code = .; _code = .; __code = .;
    *(.text)
    . = ALIGN(4096);
  }

  .data : AT(ADDR(.data) - 0xC0000000)
  {
     data = .; _data = .; __data = .;
     *(.data)
     *(.rodata)
     . = ALIGN(4096);
  }

  .bss  : AT(ADDR(.bss) - 0xC0000000)
  {
    bss = .; _bss = .; __bss = .;
    *(.bss)
    . = ALIGN(4096);
  }

  end = .; _end = .; __end = .;
}