#include "SDLTexture.h"

#include <assert.h>

SDLTexture_c::SDLTexture_c():
	pstSurface(NULL)
{	
	//empty
}

SDLTexture_c::~SDLTexture_c()
{
	SDL_FreeSurface(pstSurface);
}

void SDLTexture_c::SetSize(uint16_t w, uint16_t h)
{
	if(pstSurface)
	{
		if((pstSurface->w == w) && (pstSurface->h == h))
			return;

		SDL_FreeSurface(pstSurface);
		pstSurface = NULL;
	}

	pstSurface = SDL_CreateRGBSurface(0, w, h, 8, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
}

void *SDLTexture_c::GetPixels()
{
	assert(pstSurface != NULL);

	return pstSurface->pixels;
}

void SDLTexture_c::SetPalette(const void *data)
{
	assert(pstSurface != NULL);
	
	const uint8_t *rgb = reinterpret_cast<const uint8_t*>(data);
	SDL_Color *palette = pstSurface->format->palette->colors;
	for(int i = 0;i < 256;++i)
	{
		palette[i].r = rgb[0];
		palette[i].g = rgb[1];
		palette[i].b = rgb[2];
		palette[i].unused = 0;

		rgb += 3;
	}	
}

void SDLTexture_c::Save(const char *szFileName)
{
	SDL_SaveBMP(pstSurface, szFileName);
}
