OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
ENTRY(start)
SECTIONS
{
  . = 0x01000000;
  .text :
  {
    code = .;
    *(.text)
    *(.data)
    *(.rodata)
    *(.bss)
    . = ALIGN(512);
  }
  end = .;
}