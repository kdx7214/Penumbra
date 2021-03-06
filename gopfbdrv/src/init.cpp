#include "gopfb.h"

//
// DriverInit:  Initialize driver and return control struct
//				Get a GOP pointer from UEFI and setup for
//				font rendering using the psf font linked to
//				the driver.
//

extern "C" preload_driver *DriverInit(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) 
{
	EFI_GUID								gopguid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID ;
	EFI_GRAPHICS_OUTPUT_PROTOCOL			*gop ;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION	*info ;
	EFI_STATUS								status = EFI_ABORTED ;
	UINTN									size, num, native ;

	if (PSF_Read_Header() != 256) {
		pd.write = (preload_write)NULL ;
		pd.read = (preload_read)NULL ;
		pd.ioctl = (preload_ioctl)NULL ;
		pd.error = DRIVER_ERROR ;
		return &pd ;
	}

	pd.write = write ;
	pd.read = (preload_read)NULL ;
	pd.ioctl = (preload_ioctl)NULL ;
	pd.error = DRIVER_SUCCESS ;

	status = SystemTable->BootServices->LocateProtocol(&gopguid, NULL, (void **)&gop) ;
	if (status != EFI_SUCCESS) {
		pd.error = DRIVER_ERROR ;
		return &pd ;
	}

	fbaddr = (void *)gop->Mode->FrameBufferBase ;
	size = gop->Mode->FrameBufferSize ;
	width = gop->Mode->Info->HorizontalResolution ;
	height = gop->Mode->Info->VerticalResolution ;
	pixperscanline = gop->Mode->Info->PixelsPerScanLine ;
	pixelformat = gop->Mode->Info->PixelFormat ;

	pd.data[0] =  (uint64_t)fbaddr ;
	pd.data[1] = pixelformat ;
	pd.data[2] = pfh->width ;
	pd.data[3] = pfh->height ;
	pd.error = DRIVER_SUCCESS ;

	if (pixelformat != PixelRedGreenBlueReserved8BitPerColor && pixelformat != PixelBlueGreenRedReserved8BitPerColor) {
		pd.error = 3 ;
		return &pd ;
	}

	// Get the current mode information so we can scan the modes for native resolution
	status = gop->QueryMode(gop, gop->Mode == NULL ? 0 : gop->Mode->Mode, &size, &info) ;
	if (status == EFI_NOT_STARTED) 
		status = gop->SetMode(gop, 0) ;

	if (status != EFI_SUCCESS && native != gop->Mode->Mode) {
		pd.error = 10 ;
		return &pd ;
	}

	native = gop->Mode->Mode ;
	num = gop->Mode->MaxMode ;
	status = gop->QueryMode(gop, native, &size, &info) ;
	if (status != EFI_SUCCESS) {
		pd.error = 11 ;
		return &pd ;
	}

	status = gop->SetMode(gop, native) ;
	if (status != EFI_SUCCESS) {
		pd.error = 12 ;
		return &pd ;
	}

	fbaddr = (void *)gop->Mode->FrameBufferBase ;
	size = gop->Mode->FrameBufferSize ;
	width = gop->Mode->Info->HorizontalResolution ;
	height = gop->Mode->Info->VerticalResolution ;
	pixperscanline = gop->Mode->Info->PixelsPerScanLine ;




//  BLUE	= 0x000000ff
//  GREEN	= 0x0000ff00
//  RED		= 0x00ff0000


	int	pitch = 4 * pixperscanline ;
	unsigned int *pixptr  = (unsigned int *)fbaddr ;

	for (int x = 200; x < 600; x++) {
//		*(unsigned int *)(pixptr + x * pitch) = 0xffffffff ;
		*(unsigned int *)(pixptr + x) = 0x00ff0000 ;

	}


	pd.data[0] = (uint64_t)fbaddr ;
	pd.data[1] = pixperscanline ;
	pd.data[2] = width ;
	pd.data[3] = height ;

	pd.error = 13 ;
	return &pd ;

}

