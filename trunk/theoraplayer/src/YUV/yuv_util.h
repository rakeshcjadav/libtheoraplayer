/************************************************************************************
This source file is part of the Theora Video Playback Library
For latest info, see http://libtheoraplayer.sourceforge.net/
*************************************************************************************
Copyright (c) 2008-2014 Kresimir Spes (kspes@cateia.com)
This program is free software; you can redistribute it and/or modify it under
the terms of the BSD license: http://www.opensource.org/licenses/bsd-license.php
*************************************************************************************/
#ifndef _YUV_UTIL_h
#define _YUV_UTIL_h

#include "TheoraPixelTransform.h"

struct TheoraPixelTransform* incOut(struct TheoraPixelTransform* t, int n);
void _decodeAlpha(struct TheoraPixelTransform* t, int stride);

#endif
