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