#include <efi.h>
#include <efilib.h>
#include "elf.h"

extern EFI_FILE_HANDLE OpenFile(CHAR16* fname, EFI_HANDLE image, EFI_SYSTEM_TABLE *st) ;
extern EFI_STATUS SetPosition(EFI_FILE_HANDLE fp, UINTN position) ;
extern EFI_STATUS CloseFile(EFI_FILE_HANDLE fp) ;
extern UINT64 FileSize(EFI_FILE_HANDLE fp) ;
extern UINT8 *ReadFile(EFI_FILE_HANDLE fp, UINT64 size) ;
extern UINT8 *ReadFileAt(EFI_SYSTEM_TABLE *st, EFI_FILE_HANDLE fp, UINT64 size, void *p_addr) ;
extern int memcmp(const void *ptr_a, const void *ptr_b, UINT64 size) ;


//
// Kernel is linked at KERNEL_LOAD, but will be loaded at p_addr - KERNEL_OFFSET
// which will put it in lower half.  Before jumping to kernel need to map the
// lower memory to KERNEL_LOAD and jmp to it
//


#define KERNEL_LOAD 0xf000000000020000
#define KERNEL_OFFSET 0xf000000000000000


//
// This code is x86_64 dependent and will not work on other platforms
//


EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_FILE_HANDLE		fp ;
	Elf64Header			*elf_hdr ;

	InitializeLib(ImageHandle, SystemTable) ;
	Print(L"Attempting to load bootstrap\r\n") ;

	fp = OpenFile(L"EFI\\BOOT\\kernel.elf", ImageHandle, SystemTable) ;
	if (fp == NULL) {
		Print(L"\tError loading kernel\r\n") ;
		return EFI_ABORTED ;
	}

	if (FileSize(fp) < sizeof(Elf64Header)) {
		Print(L"\r\nInvalid ELF header!\r\n") ;
		CloseFile(fp) ;
		return EFI_ABORTED ;
	}

	Elf64Header *elf_header = (Elf64Header *)ReadFile(fp, sizeof(Elf64Header)) ;

	if (elf_header->e_mag != ELF_MAGIC) {
		Print(L"Not an ELF file\r\n") ;
		FreePool(elf_header) ;
		CloseFile(fp) ;
		return EFI_ABORTED ;
	}

	if (elf_header->e_class != ELF_64BIT) {
		Print(L"Not a 64-bit ELF file\r\n") ;
		FreePool(elf_header) ;
		CloseFile(fp) ;
		return EFI_ABORTED ;
	}

	if (elf_header->e_data != ELF_LITTLEENDIAN) {
		Print(L"Not a little endian ELF file\r\n") ;
		FreePool(elf_header) ;
		CloseFile(fp) ;
		return EFI_ABORTED ;
	}


	if (elf_header->e_type != ET_EXEC) {
		Print(L"Not an ELF executable file\r\n") ;
		FreePool(elf_header) ;
		CloseFile(fp) ;
		return EFI_ABORTED ;
	}

	if (elf_header->e_machine != ELF_AMD64) {
		Print(L"Not an AMD-64 ELF file\r\n") ;
		FreePool(elf_header) ;
		CloseFile(fp) ;
		return EFI_ABORTED ;
	}

	if (elf_header->e_version != ELF_EVCURRENT) {
		Print(L"Wrong version ELF file\r\n") ;
		FreePool(elf_header) ;
		CloseFile(fp) ;
		return EFI_ABORTED ;
	}

	if (elf_header->e_phentsize != sizeof(Elf64ProgramHeader)) {
		Print(L"Program header size does not match!\r\n") ;
		FreePool(elf_header) ;
		CloseFile(fp) ;
		return EFI_ABORTED ;
	}

	if (elf_header->e_phnum < 1) {
		Print(L"No program headers found (e_phnum == 0)!\r\n") ;
		FreePool(elf_header) ;
		CloseFile(fp) ;
		return EFI_ABORTED ;
	}


	for (int i = 0; i < elf_header->e_phnum; ++i) {
		UINTN pos = elf_header->e_phoff + (i * sizeof(Elf64ProgramHeader)) ;
		Print(L"Seeking to %lu\r\n", pos) ;
		EFI_STATUS s = SetPosition(fp, pos) ;

	}

	Print(L"\r\nProgram entry point:  %lx\r\n", elf_header->e_entry) ;
	Print(L"Program load address:  %lx\r\n", (elf_header->e_entry - KERNEL_OFFSET)) ;


	FreePool(elf_header) ;
	CloseFile(fp) ;
	Print(L"\tSuccess loading kernel\r\n") ;
	return EFI_SUCCESS ;
}

