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

	pd.write = write ;
	pd.read = (preload_read)NULL ;
	pd.ioctl = (preload_ioctl)NULL ;
	pd.error = 0 ;

	status = SystemTable->BootServices->LocateProtocol(&gopguid, NULL, (void **)&gop) ;
	//status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopguid, NULL, (void **)&gop) ;
	if (status != EFI_SUCCESS) {
		pd.error = 1 ;
		return &pd ;
	}

	fbaddr = (void *)gop->Mode->FrameBufferBase ;
	size = gop->Mode->FrameBufferSize ;
	width = gop->Mode->Info->HorizontalResolution ;
	height = gop->Mode->Info->VerticalResolution ;
	pixperscanline = gop->Mode->Info->PixelsPerScanLine ;

	//pd.data[0] = (uint64_t)fbaddr ;
	pd.data[0] = (uint64_t)&_binary_src_zap_light24_psf_start ;
	pd.data[1] = (uint64_t)&_binary_src_zap_light24_psf_end ;
	pd.data[2] = width ;
	pd.data[3] = height ;
	pd.error = 777 ;

	// *((uint32_t*)(fbaddr + 4 * pixperscanline * 400 + 600 + 4 * 32)) = 0xffff00 ;


	// Get the current mode information so we can scan the modes for native resolution
	status = gop->QueryMode(gop, gop->Mode == NULL ? 0 : gop->Mode->Mode, &size, &info) ;
	if (status == EFI_NOT_STARTED) 
		status = gop->SetMode(gop, 0) ;

	if (status != EFI_SUCCESS)				// On error, just return current video mode information
		return &pd ;

	if (native != gop->Mode->Mode)			// Already in native resolution
		return &pd ;

	native = gop->Mode->Mode ;
	num = gop->Mode->MaxMode ;
	status = gop->QueryMode(gop, native, &size, &info) ;
	if (status != EFI_SUCCESS)
		return &pd ;

	status = gop->SetMode(gop, native) ;
	if (status != EFI_SUCCESS)
		return &pd ;

	fbaddr = (void *)gop->Mode->FrameBufferBase ;
	size = gop->Mode->FrameBufferSize ;
	width = gop->Mode->Info->HorizontalResolution ;
	height = gop->Mode->Info->VerticalResolution ;
	pixperscanline = gop->Mode->Info->PixelsPerScanLine ;

	pd.data[0] =  (uint64_t)fbaddr ;
	pd.data[1] = size ;
	pd.data[2] = width ;
	pd.data[3] = height ;
	pd.error = 123 ;

	return &pd ;

}

