/*
wadlib
Copyright (c) 2011 Bruno Sanches  http://code.google.com/p/wadlib

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/


#ifndef ITEXTURE_H
#define ITEXTURE_H

#include "WadTypes.h"

class ITexture_c
{
	public:
		virtual ~ITexture_c(){};

		virtual void SetSize(uint16_t w, uint16_t h) = 0;
		virtual void *GetPixels() = 0;

		virtual void SetPalette(const void *) = 0;
};

#endif