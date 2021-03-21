//
// PSF:   Load and process PSF information embedded in ELF file
//

#include "gopfb.h"

int PSF_Read_Header(void) {

	pfh = (psf_header *)&_binary_src_zap_light24_psf_start ;		// This font is 10x24 pixels

	if (pfh->magic != PSF_MAGIC || pfh->numglyphs != 256)
		return 0 ;

	return 256 ;
}


