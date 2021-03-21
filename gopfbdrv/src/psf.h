//
// PSF:  PSF format for bitmapped font files
//

#pragma once
#ifndef __PSF_H
#define __PSF_H

#define PSF_MAGIC	0x864ab572

typedef struct {
	uint32_t	magic ;
	uint32_t	version ;
	uint32_t	headersize ;
	uint32_t	flags ;
	uint32_t	numglyphs ;
	uint32_t	bytesperglyph ;
	uint32_t	height ;
	uint32_t	width ;
} psf_header ;

extern int PSF_Read_Header(void) ;

#endif

