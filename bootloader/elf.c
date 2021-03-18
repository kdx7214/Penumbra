#include <efi.h>
#include <efilib.h>
#include "elf.h"

extern EFI_FILE_HANDLE OpenFile(CHAR16 *fname, EFI_HANDLE image, EFI_SYSTEM_TABLE *st) ;
extern EFI_STATUS SetPosition(EFI_FILE_HANDLE fp, UINTN position) ;
extern EFI_STATUS CloseFile(EFI_FILE_HANDLE fp) ;
extern UINT64 FileSize(EFI_FILE_HANDLE fp) ;
extern UINT8 *ReadFile(EFI_FILE_HANDLE fp, UINT64 size) ;
extern UINT8 *ReadFileAt(EFI_SYSTEM_TABLE *st, EFI_FILE_HANDLE fp, UINT64 size, void *p_addr) ;

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


//
// ReadElf:	Read en elf linked file into memory.  If it's load address is
//			above KERNEL_OFFSET then relocate to paddr - KERNEL_OFFSET
//

UINT64 ReadElf(CHAR16 *fname, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_FILE_HANDLE		fp ;
	Elf64Header			*elf_hdr ;
	UINT64				entry ;

	Print(L"ReadElf:   Starting\r\n") ;

	Print(L"           Opening file\r\n") ;
	fp = OpenFile(fname, ImageHandle, SystemTable) ;
	if (fp == NULL) {
		Print(L"     Error opening ELF file.\r\n") ;
		return 0 ;
	}

	Print(L"           Verifying file size\r\n") ;
	if (FileSize(fp) < sizeof(Elf64Header)) {
		Print(L"     Invalid ELF header.\r\n") ;
		CloseFile(fp) ;
		return 0 ;
	}

	Print(L"           Reading ELF header\r\n") ;
	Elf64Header *elf_header = (Elf64Header *)ReadFile(fp, sizeof(Elf64Header)) ;
	if (check_elf(elf_header) != EFI_SUCCESS) {
		FreePool(elf_header) ;
		CloseFile(fp) ;
		return 0 ;
	}

	Elf64ProgramHeader	*ph ;
	int					abort = 0 ;

	Print(L"           Reading ELF program headers\r\n") ;
	for (int i = 0; i < elf_header->e_phnum; ++i) {
		UINTN pos = elf_header->e_phoff + (i * sizeof(Elf64ProgramHeader)) ;

		if (SetPosition(fp, pos) != EFI_SUCCESS) { 
			FreePool(elf_header) ;
			CloseFile(fp) ;
			return 0 ;
		}
		ph = (Elf64ProgramHeader *)ReadFile(fp, sizeof(Elf64ProgramHeader)) ;
		if (ph->p_type != PT_LOAD) {
			FreePool(ph) ;
			continue ;
		}

		if (SetPosition(fp, ph->p_offset) != EFI_SUCCESS) {
			Print(L"     Error setting position\r\n") ;
			abort++ ;
		}

		Print(L"           Reading ELF program data\r\n") ;
		UINT8 *r = ReadFileAt(SystemTable, fp, ph->p_filesz, (void *)ph->p_paddr) ;
		if (r != (UINT8 *)ph->p_paddr) {
			Print(L"     Address mismatch\r\n") ;
			abort++ ;
		}
		FreePool(ph) ;
	} // for (...)

	entry = elf_header->e_entry ;
	FreePool(elf_header) ;
	CloseFile(fp) ;
	if (abort) {
		Print(L"     Error loading kernel.\r\n") ;
		return 0 ;
	}

	return entry ;

}

