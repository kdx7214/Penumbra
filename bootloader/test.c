#include <stdio.h>

typedef unsigned char UINT8 ;
typedef unsigned short UINT16 ;
typedef unsigned int UINT32 ;
typedef unsigned long long UINT64 ;

#include "elf.h"

int main(int argc, char **argv)
{

	Elf64Header			elf ;
	Elf64ProgramHeader	eph ;
	FILE*				fp ;

	if (argc != 2) {
		printf("Usage:  %s progname\n", argv[0]) ;
		return -1 ;
	}

	fp = fopen(argv[1], "rb") ;
	fread((void *)&elf, sizeof(elf), 1, fp) ;

	printf("Size of header:  %ld\n", sizeof(elf)) ;

#if 0
	printf("e_mag:        0x%x\n", elf.e_mag) ;
	printf("e_class:      %d\n", elf.e_class) ;
	printf("e_data:       %d\n", elf.e_data) ;
	printf("e_version:    %d\n", elf.e_version) ;
	printf("e_ident:      %d\n", elf.e_ident) ;
	printf("e_osabi:      %d\n", elf.e_osabi) ;
	printf("e_abiversion: %d\n", elf.e_abiversion) ;
	printf("e_type:       %d\n", elf.e_type) ;
	printf("e_machine:    0x%X\n", elf.e_machine) ;
	printf("e_entry:      0x%llX\n", elf.e_entry) ;
	printf("e_phoff:      %lld\n", elf.e_phoff) ;
	printf("e_shoff:      %lld\n", elf.e_shoff) ;
	printf("e_flags:      %d\n", elf.e_flags) ;
	printf("e_ehsize:     %d\n", elf.e_ehsize) ;
	printf("e_phentsize:  %d\n", elf.e_phentsize) ;
	printf("e_phnum:      %d\n", elf.e_phnum) ;
	printf("e_shentsize:  %d\n", elf.e_shentsize) ;
	printf("e_shnum:      %d\n", elf.e_shnum) ;
	printf("e_shstrndx    %d\n", elf.e_shstrndx) ;
#endif

	if (sizeof(Elf64ProgramHeader) != elf.e_phentsize)
		printf("Program header size mismatch\n") ;

	if (sizeof(Elf64SectionHeader) != elf.e_shentsize)
		printf("Section header size mismatch\n") ;

	for (int i = 0; i < elf.e_phnum; ++i)
	{
		printf("Seeking to:  %lld\n", elf.e_phoff + (i * elf.e_phentsize)) ;
		fseek(fp, elf.e_phoff + (i * elf.e_phentsize), 0) ;
		fread((void *)&eph, sizeof(eph), 1, fp) ;
		printf("{ 0x%X, %d, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX, 0x%llX }\n",
			eph.p_type, eph.p_flags, eph.p_offset, eph.p_vaddr, eph.p_paddr, eph.p_filesz, eph.p_memsz, eph.p_align) ;

	}


	fclose(fp) ;

}

