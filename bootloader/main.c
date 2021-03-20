/*

	Penumbra:	A simplistic attempt at a pseudo-microkernel design

	Kernel:		Loads at 2mb (as defined in elf file) and is virtually
				mapped to a higher-half kernel before starting.

	Drivers:	Some very elementary drivers are loaded by the bootloader
				and passed in a struct to the kernel.  Temp drivers like
				this do not get relocated as they will eventually be replaced
				by real hardware drivers.

	gopfbdrv:	UEFI Graphics Output Protocol framebuffer driver for bootloader
				and early kernel use.  Loads somewhere between 1mb and < 2mb as
				indicated in .elf file.  This will be accessible from the kernel
				after exiting BootServices.


*/

#include <efi.h>
#include <efilib.h>
#include <preload.h>
#include "elf.h"

extern EFI_FILE_HANDLE OpenFile(CHAR16 *fname, EFI_HANDLE image, EFI_SYSTEM_TABLE *st) ;
extern EFI_STATUS SetPosition(EFI_FILE_HANDLE fp, UINTN position) ;
extern EFI_STATUS CloseFile(EFI_FILE_HANDLE fp) ;
extern UINT64 FileSize(EFI_FILE_HANDLE fp) ;
extern UINT8 *ReadFile(EFI_FILE_HANDLE fp, UINT64 size) ;
extern UINT8 *ReadFileAt(EFI_SYSTEM_TABLE *st, EFI_FILE_HANDLE fp, UINT64 size, void *p_addr) ;
extern EFI_STATUS check_elf(Elf64Header *elf) ;
extern UINT64 ReadElf(CHAR16 *fname, EFI_HANDLE image, EFI_SYSTEM_TABLE *st) ;


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
	UINT64				entry, dentry ;

	InitializeLib(ImageHandle, SystemTable) ;


	Print(L"\r\nLoading GOP driver\r\n") ;
	dentry = ReadElf(L"EFI\\BOOT\\gopfbdrv.elf", ImageHandle, SystemTable) ;
	if (dentry == 0)
	{
		Print(L"Error loading gop driver\r\n") ;
		return EFI_ABORTED ;
	}
	preload_driver *(*DriverInit)(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) = ((__attribute__((sysv_abi)) preload_driver *(*)(EFI_HANDLE, EFI_SYSTEM_TABLE *) ) dentry) ;
	preload_driver *gopfb = DriverInit(ImageHandle, SystemTable) ;
	Print(L"Successfully loaded GOP\r\n") ;

#ifdef __DEBUG
	Print(L"\r\nDriver entry point:  %lx\r\n", (UINTN)dentry) ;
	Print(L"gop data[0]:  0x%lx\r\n", gopfb->data[0]) ;
	Print(L"gop data[1]:  %lu\r\n", gopfb->data[1]) ;
	Print(L"gop data[2]:  %lu\r\n", gopfb->data[2]) ;
	Print(L"gop data[3]:  %lu\r\n", gopfb->data[3]) ;
	Print(L"gop error:  %lu\r\n", gopfb->error) ;
#endif


	Print(L"Loading kernel...\r\n") ;
	entry = ReadElf(L"EFI\\BOOT\\kernel.elf", ImageHandle, SystemTable) ;
	if (entry == 0) 
	{
		Print(L"Error loading kernel.  Aborting.\r\n") ;
		return EFI_ABORTED ;
	}
	Print(L"Successfully loaded kernel\r\n") ;
	Print(L"Kernel entry point:  %lx\r\n", (UINTN)entry) ;


	//
	// Get system memory map
	//
	
	b_param		bp ;
	UINTN		mkey ;
	UINT32		dversion ;

	//SystemTable->BootServices->GetMemoryMap(&bp.msize, bp.map, &mkey, &bp.dsize, &dversion) ;
	//SystemTable->BootServices->AllocatePool(EfiLoaderData, bp.msize, (void **)&bp.map) ;
	//SystemTable->BootServices->GetMemoryMap(&bp.msize, bp.map, &mkey, &bp.dsize, &dversion) ;



	int (*KernelMain)(b_param *) = ((__attribute__((sysv_abi)) int (*)(b_param *) ) entry) ;
	//SystemTable->BootServices->ExitBootServices(ImageHandle, mkey) ;
	Print(L"Starting kernel...\r\n") ;
	int ret = KernelMain(&bp) ;
	Print(L"Return:  %d\r\n", ret) ;

	return EFI_SUCCESS ;
}

