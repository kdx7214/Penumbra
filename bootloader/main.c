#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
//	InitializeLib(ImageHandle, SystemTable) ;
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Testing...\r\n") ;
	return EFI_ABORTED ;
}

