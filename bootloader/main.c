#include <efi.h>
#include <efilib.h>
#include "elf.h"

extern EFI_FILE_HANDLE OpenFile(CHAR16* fname, EFI_HANDLE image, EFI_SYSTEM_TABLE *st) ;
extern EFI_STATUS SetPosition(EFI_FILE_HANDLE fp, UINTN position) ;
extern EFI_STATUS CloseFile(EFI_FILE_HANDLE fp) ;
extern UINT64 FileSize(EFI_FILE_HANDLE fp) ;
extern UINT8 *ReadFile(EFI_FILE_HANDLE fp, UINT64 size) ;
extern UINT8 *ReadFileAt(EFI_SYSTEM_TABLE *st, EFI_FILE_HANDLE fp, UINT64 size, void *p_addr) ;
extern EFI_STATUS check_elf(Elf64Header *elf) ;

//
// Kernel is linked at KERNEL_LOAD, but will be loaded at p_addr - KERNEL_OFFSET
// which will put it in lower half.  Before jumping to kernel need to map the
// lower memory to KERNEL_LOAD and jmp to it.
//


#define KERNEL_LOAD			0xf000000000020000
#define KERNEL_OFFSET		0xf000000000000000
#define KERNEL_MEM_PAGES	1024						// Pages to reserve (2MB of ram)
#define KERNEL_MEM_START	0x100000					// Start of region to reserve
#define KERNEL_MEM_END		0x2FFFFF					// End of kernel region


//
// b_param:	Pass parameters needed from UEFI to the kernel
//

	typedef struct {
		EFI_MEMORY_DESCRIPTOR	*map ;					// Memory map
		UINTN					msize ;					// Map size
		UINTN					dsize ;					// Descriptor size
	} b_param ;


//
// This code is x86_64 dependent and will not work on other platforms
//

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_FILE_HANDLE		fp ;
	Elf64Header			*elf_hdr ;

	InitializeLib(ImageHandle, SystemTable) ;
	Print(L"Attempting to load kernel\r\n") ;

	fp = OpenFile(L"EFI\\BOOT\\kernel.elf", ImageHandle, SystemTable) ;
	if (fp == NULL) {
		Print(L"     Error loading kernel\r\n") ;
		return EFI_ABORTED ;
	}

	if (FileSize(fp) < sizeof(Elf64Header)) {
		Print(L"\r\nInvalid ELF header!\r\n") ;
		CloseFile(fp) ;
		return EFI_ABORTED ;
	}

	Elf64Header *elf_header = (Elf64Header *)ReadFile(fp, sizeof(Elf64Header)) ;
	if (check_elf(elf_header) != EFI_SUCCESS) {
		FreePool(elf_header) ;
		CloseFile(fp) ;
		return EFI_ABORTED ;
	}

	Elf64ProgramHeader	*ph ;
	int					abort = 0 ;

	Print(L"# ph:  %u\r\n", elf_header->e_phnum) ;

	for (int i = 0; i < elf_header->e_phnum; ++i) {
		UINTN pos = elf_header->e_phoff + (i * sizeof(Elf64ProgramHeader)) ;

		if (SetPosition(fp, pos) != EFI_SUCCESS) { 
			FreePool(elf_header) ;
			CloseFile(fp) ;
			return EFI_ABORTED ;
		}
		ph = (Elf64ProgramHeader *)ReadFile(fp, sizeof(Elf64ProgramHeader)) ;
		if (ph->p_type != PT_LOAD) {
			Print(L"    Not a PT_LOAD segment\r\n") ;
			FreePool(ph) ;
			continue ;
		}

		Print(L"     Loading kernel at:  0x%lx\r\n", (UINTN)ph->p_paddr) ;
		Print(L"     Seeking to:   0x%ld\r\n", ph->p_offset) ;
		if (SetPosition(fp, ph->p_offset) != EFI_SUCCESS) {
			Print(L"Error setting position\r\n") ;
			abort++ ;
		}
		UINT8 *r = ReadFileAt(SystemTable, fp, ph->p_filesz, (void *)ph->p_paddr) ;
		Print(L"     Return from ReadFileAt:  0x%lx\r\n", (UINTN)r) ;
		if (r != (UINT8 *)ph->p_paddr) {
			Print(L"Address mismatch\r\n") ;
			abort++ ;
		}
		FreePool(ph) ;
		Print(L"After FreePool\r\n") ;

	} // for (...)

//	FreePool(elf_header) ;
	CloseFile(fp) ;
	if (abort == 0)
		Print(L"Successfully loaded kernel\r\n") ;
	Print(L"Abort == %d\r\n", abort) ;


	//
	// Get system memory map
	//
	
	b_param		bp ;
	UINTN		mkey ;
	UINT32		dversion ;

	SystemTable->BootServices->GetMemoryMap(&bp.msize, bp.map, &mkey, &bp.dsize, &dversion) ;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, bp.msize, (void **)&bp.map) ;
	SystemTable->BootServices->GetMemoryMap(&bp.msize, bp.map, &mkey, &bp.dsize, &dversion) ;


	Print(L"\r\nProgram entry point:  %lx\r\n", (UINTN)elf_header->e_entry) ;
	//Print(L"Program load address:  %lx\r\n", (UINTN)(elf_header->e_entry - KERNEL_OFFSET)) ;



	int (*KernelMain)(b_param *) = ((__attribute__((sysv_abi)) int (*)(b_param *) ) elf_header->e_entry) ;
	//SystemTable->BootServices->ExitBootServices(ImageHandle, mkey) ;
	if (!abort) {
		int ret = KernelMain(&bp) ;
		Print(L"Return:  %d\r\n", ret) ;
	}

	return EFI_SUCCESS ;
}

