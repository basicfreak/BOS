OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
TARGET(elf32-i386)
ENTRY(init)
TextPhys = 0x01000000;
DataPhys = 0x41000000;
BSSPhys = 0x61000000;
SECTIONS
{
  .text TextPhys : AT(TextPhys)
  {
    code = .; _code = .; __code = .;
    *(.text)
    . = ALIGN(4096);
  }

  .data DataPhys : AT(DataPhys)
  {
     data = .; _data = .; __data = .;
     *(.data)
     *(.rodata)
    . = ALIGN(4096);
  }

  .bss BSSPhys : AT(BSSPhys)
  {
    bss = .; _bss = .; __bss = .;
    *(.bss)
    . = ALIGN(4096);
  }

  /DISCARD/ : {
      *(.comment)
    }

  end = .; _end = .; __end = .;
}