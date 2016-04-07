OUTPUT_FORMAT("binary")
ENTRY(start)
phys = 0xFFFFFFFFC0110000;
SECTIONS
{
  FILE_IO = 0xA000;
  AP_Strap = 0x8800;
  _PMM_init = 0xFFFFFFFFC0100000;
  _PMM_free = 0xFFFFFFFFC0101000;
  _PMM_alloc = 0xFFFFFFFFC0101800;
  _VMM_init = 0xFFFFFFFFC0108000;
  _VMM_umap = 0xFFFFFFFFC0109000;
  _VMM_map = 0xFFFFFFFFC0109800;

  .text phys : AT(phys) {
    text = .;
    *(.text)
    . = ALIGN(0x1000);
  }
  .data : AT(phys + (data - text)) {
    data = .;
    *(.data)
    *(.bss)
    . = ALIGN(0x1000);
  }
  .rodata : AT(phys + (rodata - text)) {
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

