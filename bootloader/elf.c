#include <efi.h>
#include <efilib.h>
#include "elf.h"

//
// elf_check:  Do sanity checks on the ELF header
//

EFI_STATUS check_elf(Elf64Header *elf)
{
	if (elf->e_mag != ELF_MAGIC) {
		Print(L"Not an ELF file\r\n") ;
		return EFI_ABORTED ;
	}

	if (elf->e_class != ELF_64BIT) {
		Print(L"Not a 64-bit ELF file\r\n") ;
		return EFI_ABORTED ;
	}

	if (elf->e_data != ELF_LITTLEENDIAN) {
		Print(L"Not a little endian ELF file\r\n") ;
		return EFI_ABORTED ;
	}


	if (elf->e_type != ET_EXEC) {
		Print(L"Not an ELF executable file\r\n") ;
		return EFI_ABORTED ;
	}

	if (elf->e_machine != ELF_AMD64) {
		Print(L"Not an AMD-64 ELF file\r\n") ;
		return EFI_ABORTED ;
	}

	if (elf->e_version != ELF_EVCURRENT) {
		Print(L"Wrong version ELF file\r\n") ;
		return EFI_ABORTED ;
	}

	if (elf->e_phentsize != sizeof(Elf64ProgramHeader)) {
		Print(L"Program header size does not match!\r\n") ;
		return EFI_ABORTED ;
	}

	if (elf->e_phnum < 1) {
		Print(L"No program headers found (e_phnum == 0)!\r\n") ;
		return EFI_ABORTED ;
	}

	return EFI_SUCCESS ;
}

