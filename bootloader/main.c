#include <efi.h>
#include <efilib.h>
#include "elf.h"


extern EFI_FILE_HANDLE OpenFile(CHAR16* fname, EFI_HANDLE image, EFI_SYSTEM_TABLE *st) ;
extern EFI_STATUS SetPosition(EFI_FILE_HANDLE fp, UINTN position) ;
extern EFI_STATUS CloseFile(EFI_FILE_HANDLE fp) ;
extern UINT64 FileSize(EFI_FILE_HANDLE fp) ;
extern UINT8 *ReadFile(EFI_FILE_HANDLE fp, UINT64 size) ;
extern int memcmp(const void *ptr_a, const void *ptr_b, UINT64 size) ;

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_FILE_HANDLE		fp ;

	InitializeLib(ImageHandle, SystemTable) ;
	Print(L"Attempting to load bootstrap\r\n") ;

	fp = OpenFile(L"EFI\\BOOT\\kernel.elf", ImageHandle, SystemTable) ;
	if (fp == NULL) {
		Print(L"\tError loading kernel\r\n") ;
		return EFI_ABORTED ;
	}
	UINT64 fsize = FileSize(fp) ;
	Print(L"File size:  %u\r\n", fsize) ;


	CloseFile(fp) ;
	Print(L"\tSuccess opening bootstrap\r\n") ;
	return EFI_SUCCESS ;
}

