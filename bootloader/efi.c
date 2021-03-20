#include <efi.h>
#include <efilib.h>
#include "elf.h"


//
// OpenFile:	Open a file for reading from the base directory of the volume (\EFI\BOOT)
//
EFI_FILE_HANDLE OpenFile(CHAR16* fname, EFI_HANDLE image, EFI_SYSTEM_TABLE *st) {
	EFI_LOADED_IMAGE	*li = NULL ;
	EFI_GUID			lip = LOADED_IMAGE_PROTOCOL ;
	EFI_STATUS			r ;
	EFI_FILE_HANDLE		volume, f ;

	st->BootServices->HandleProtocol(image, &lip, (void **)&li) ;
	if (li == NULL) {
		Print(L"     Error getting LoadedImage\r\n") ;
		return NULL ;
	}

	volume = LibOpenRoot(li->DeviceHandle) ;
	if (volume == NULL) {
		Print(L"     Error getting volume root\r\n") ;
		return NULL ;
	}

	r = volume->Open(volume, &f, fname, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY) ;
	if (r != EFI_SUCCESS) {
		Print(L"     Error opening file (%d)\r\n", r) ;
		return NULL ;
	}

	return f ;
}


//
// SetPosition:	Set file pointer position for reads
//
EFI_STATUS SetPosition(EFI_FILE_HANDLE fp, UINTN position) {
	if (fp == NULL)
		return EFI_ABORTED ;
	return fp->SetPosition(fp, position) ;
}



//
// CloseFile:	Close the file
//
EFI_STATUS CloseFile(EFI_FILE_HANDLE fp) {
	fp->Close(fp);
}


//
// FileSize:	Get file size
//
UINT64 FileSize(EFI_FILE_HANDLE fp) {
	UINT64		r ;
	EFI_FILE_INFO	*fi ;
	fi = LibFileInfo(fp) ;
	r = fi->FileSize ;
	FreePool(fi) ;
	return r ;
}


//
// ReadFile:	Read data from a file.  Position is set via SetPosition()
//				Caller must free the buffer with FreePool(buffer)
//

UINT8 *ReadFile(EFI_FILE_HANDLE fp, UINT64 size) {
	UINT64	read_size = size ;
	UINT8	*buffer = AllocatePool(size) ;

	if (buffer == NULL)
		return NULL ;

	fp->Read(fp, &read_size, buffer) ;
	return buffer ;
}


//
// ReadFileAt:	Read file at a particular point in memory, allowing the
//				request of a number of pages
//
UINT8 *ReadFileAt(EFI_SYSTEM_TABLE *st, EFI_FILE_HANDLE fp, UINT64 size, void *p_addr) {
	UINT64 pages = (size + 0x1000 - 1) / 0x1000 ;
	UINT64		sz = size ;
	EFI_PHYSICAL_ADDRESS p = (EFI_PHYSICAL_ADDRESS)p_addr ;
	st->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, p_addr) ;
	fp->Read(fp, &sz, p_addr) ;
	return (UINT8 *)p ;
}

