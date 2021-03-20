#include <preload.h>

//
// Global variables used in this driver.  These can be used internally or
// passed back to the bootloader/kernel.
//

preload_driver		pd ;
void				*fbaddr ;
uint64_t			size, width, height, pixperscanline ;

