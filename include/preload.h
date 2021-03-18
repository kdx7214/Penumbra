/*

	preload.h:		Structures and manifest constants for preload drivers initially used by the kernel.  After full
					drivers for devices are written, these will be reclaimed for usable memory.  Preload drivers
					do not get relocated to higher half.

					The entry point for the elf file should be:
					preload_driver *driver_init(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) ;

					Since the driver is statically linked at a fixed address, it 
					can return a preload_driver* that is local to the driver.
					
*/

#pragma once

#include <efi.h>
#include <efilib.h>

typedef uint64_t (*preload_read)(void *buffer, uint64_t count) ;		// Returns # bytes read into buffer
typedef uint64_t (*preload_write)(void *buffer, uint64_t count) ;		// Returns # bytes written from buffer to device
typedef uint64_t (*preload_ioctl)(uint64_t request, ...) ;				// Control the device


//
// preload_driver:	Contains the definitions needed for the kernel to communicate with the device
//					A pointer to this structure is returned from the driver's init function.
//

typedef struct {
	preload_read	read ;
	preload_write	write ;
	preload_ioctl	ioctl ;
	uint64_t		error ;
	uint64_t		data[4] ;
} preload_driver ;

