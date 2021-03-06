#pragma once

#ifndef _GOP_FB_H
#define _GOP_FB_H

typedef unsigned int uint32_t ;


#include <preload.h>
#include "psf.h"

extern preload_driver	pd ;
extern void				*fbaddr ;
extern uint64_t			size, width, height, pixperscanline ;
extern uint64_t			pixelformat ;
extern psf_header		*pfh ;

extern uint8_t			_binary_src_zap_light24_psf_start ;
extern uint8_t			_binary_src_zap_light24_psf_end ;
extern uint8_t			_binary_src_zap_light24_psf_size ;


extern uint64_t write(void *buffer, uint64_t size) ;
extern uint64_t setpixel(int x, int y, int color) ;


#endif


