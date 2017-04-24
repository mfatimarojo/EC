

SECTIONS
{
    . = 0x0C100000;
    .data : {
      *(.data*)
      *(.rodata*) 
    }
	Image_ZI_Base = .;
    .bss : { 
      *(.bss*)
      *(COMMON)
    }
	Image_ZI_Limit = .;
    .text : { 
      *(.text*)
    }
    PROVIDE( end = . );
}
GROUP(
   libgcc.a
   libg.a
   libc.a
   libm.a
   libnosys.a
 )
 
