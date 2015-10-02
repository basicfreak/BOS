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
    *(.text.*)
    . = ALIGN(0);
  }
  .rodata : AT(Phys + (rodata - code))
  {
    rodata = .; _rodata = .; __rodata = .;
    *(.rodata)
    *(.eh_frame)
    . = ALIGN(0);
  }

  /DISCARD/ : {
      *(.data)
      *(.bss)
      *(.comment)
      *(.note.*)
    }

  end = .; _end = .; __end = .;
}