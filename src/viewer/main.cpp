#include <SDL.h>
#include <SDL_gfxPrimitives.h>

#include <assert.h>
#include <iostream>
#include <set>

#include "ITexture.h"
#include "WadFile.h"
#include "WadLevel.h"
#include "Vector2d.h"

enum DrawFlags_e
{
	DF_SHOW_LINE_SIDES = 0x01,
	DF_SHOW_SEGMENTS = 0x02,
	DF_SHOW_VERTICES = 0x04,
	DF_SHOW_PARTITION_LINES = 0x08,
	DF_SHOW_SUBSECTORS = 0x10,
	DF_SHOW_THINGS = 0x20
};

//"C:\games\DOOM Ultimate\Doom.wad" E1M1 E1M2 E1M3 E1M4 E1M5 E1M6 E1M7 E1M8 E1M9 E2M1 E2M2 E2M3 E2M4 E2M5 E2M6 E2M7 E2M8 E2M9 E3M1 E3M2 E3M3 E3M4 E3M5 E3M6 E3M7 E3M8 E3M9 E4M1 E4M2 E4M3 E4M4 E4M5 E4M6 E4M7 E4M8 E4M9
//"C:\games\DOOM 2\doom2.wad" MAP01 MAP02 MAP03 MAP04 MAP05 MAP06 MAP07 MAP08 MAP09 MAP10 MAP11 MAP12 MAP13 MAP14 MAP15 MAP16 MAP17 MAP18 MAP19 MAP20 MAP21 MAP22 MAP23 MAP24 MAP25 MAP26 MAP27 MAP28 MAP29 MAP30 MAP31 MAP32

class SDLTexture_c: public ITexture_c
{
	public:
		SDLTexture_c();
		virtual ~SDLTexture_c();

		virtual void SetSize(uint16_t w, uint16_t h);
		virtual void *GetPixels();
		virtual void SetPalette(const void *);

		void Save(const char *szFileName);
	private:
		SDL_Surface *pstSurface;
};

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

struct FlatNameFunctor_s
{
	bool operator()(const char *pchLhs, const char *pchRhs)const
	{
		return strncmp(pchLhs, pchRhs, 8) < 0;
	}
};

typedef std::set<const char *, FlatNameFunctor_s> FlatNameSet_t;

static void ExportFlat(SDLTexture_c &texture,  WadFile_c &file, FlatNameSet_t &setExportedFlats,  const char *pchName)
{
	FlatNameSet_t::iterator it = setExportedFlats.lower_bound(pchName);
	if((it != setExportedFlats.end()) && (!setExportedFlats.key_comp()(pchName, *it)))
	{
		//already exists, ignore
	}
	else
	{
		file.LoadFlat(texture, pchName);

		setExportedFlats.insert(it, pchName);		

		char szFileName[13] = {0};
		memcpy(szFileName, pchName, 8);
		strcat(szFileName, ".bmp");

		std::cout << szFileName << std::endl;
		texture.Save(szFileName);
	}
}

static void ExportFlats(const WadLevel_c &level, WadFile_c &file)
{
	FlatNameSet_t setExportedFlats;

	SDLTexture_c texture;

	std::cout << "Exporting current level flats:" << std::endl;
	const Sector_s *sectors = level.GetSectors();
	for(size_t i = 0;i < level.GetNumSectors(); ++i)
	{
		const Sector_s &sector = sectors[i];

		ExportFlat(texture, file, setExportedFlats, sector.archFloorTexture);
		ExportFlat(texture, file, setExportedFlats, sector.archCeilTexture);				
	}
	std::cout << "Done." << std::endl;
}

static void ExportAllFlats(WadFile_c &file)
{
	FlatNameSet_t setExportedFlats;

	SDLTexture_c texture;

	std::cout << "Exporting WAD flats:" << std::endl;
	for(const Directory_s *flat = file.FlatBegin(), *end = file.FlatEnd(); flat != end; ++flat)
	{
		ExportFlat(texture, file, setExportedFlats, flat->archName);
	}
	std::cout << "Done." << std::endl;
}

static void DrawLevel(const WadLevel_c &level, SDL_Surface *screen, uint32_t flags)
{
	float scaleX, scaleY;
	float offsetX, offsetY;

	const Vertex_s &mapMin = level.GetMin();
	const Vertex_s &mapMax = level.GetMax();

	scaleX = (float)screen->w / ((float)(mapMax.iX - mapMin.iX) * 1.05f);
	scaleY = (float)screen->h / ((float)(mapMax.iY - mapMin.iY) * -1.05f);
	
	offsetX = -mapMin.iX * scaleX;
	offsetY = (-mapMin.iY * scaleY) + screen->h;

	const uint32_t red = 0xFF0000FF;
	const uint32_t white = 0xFFFFFFFF;
	const uint32_t yellow = 0xFFFF00FF;

	const LineDef_s *lineDefs = level.GetLineDefs();
	const size_t numLineDefs = level.GetNumLineDefs();

	const Vertex_s *vertices = level.GetVertices();
	for(size_t i = 0;i < numLineDefs; ++i)
	{
		const Vertex_s &a = vertices[lineDefs[i].iStartVertex];
		const Vertex_s &b = vertices[lineDefs[i].iEndVertex];

		float x1 = (a.iX * scaleX) + offsetX;
		float y1 = (a.iY * scaleY) + offsetY;
		float x2 = (b.iX * scaleX) + offsetX;
		float y2 = (b.iY * scaleY) + offsetY;

		uint32_t color = (lineDefs[i].iLeftSideDef < 0) ? white : red;		
		color = lineDefs[i].iSpecialType > 0 ? yellow : color;
		lineColor(screen, (int16_t)x1, (int16_t)y1, (int16_t)x2, (int16_t)y2, color);			

		float midX = (a.iX + b.iX) / 2.0f;
		float midY = (a.iY + b.iY) / 2.0f;

		if(flags & DF_SHOW_LINE_SIDES)
		{
			Vector2d_s v(Vector2d_s(b.iX, b.iY), Vector2d_s(a.iX, a.iY));
			v.Normalize();		
			v.Rotate90();
			v.Scale(20.0f);
			v.Add(Vector2d_s(midX, midY));		

			x1 = (midX * scaleX) + offsetX;
			y1 = (midY * scaleY) + offsetY;
			x2 = (v.fX * scaleX) + offsetX;
			y2 = (v.fY * scaleY) + offsetY;			

			lineColor(screen, (int16_t)x1, (int16_t)y1, (int16_t)x2, (int16_t)y2, color);
		}		

		if(flags & DF_SHOW_VERTICES)
		{
			filledCircleColor(screen, (int16_t)x1, (int16_t)y1, 1, 0x0000FFFF);
			filledCircleColor(screen, (int16_t)x2, (int16_t)y2, 1, 0x0000FFFF);
		}
	}
	if(flags & DF_SHOW_SEGMENTS)
	{
		const Segment_s *segments = level.GetSegments();
		const size_t numSegments = level.GetNumSegments();
		for(size_t i = 0;i < numSegments; ++i)
		{
			const Vertex_s &a = vertices[segments[i].iStartVertex];
			const Vertex_s &b = vertices[segments[i].iEndVertex];

			float x1 = (a.iX * scaleX) + offsetX;
			float y1 = (a.iY * scaleY) + offsetY;
			float x2 = (b.iX * scaleX) + offsetX;
			float y2 = (b.iY * scaleY) + offsetY;
									
			filledCircleColor(screen, (int16_t)x1, (int16_t)y1, 1, 0x00FF00FF);
			filledCircleColor(screen, (int16_t)x2, (int16_t)y2, 1, 0x00FF00FF);
		}
	}

	if(flags & DF_SHOW_PARTITION_LINES)
	{
		const Node_s *nodes = level.GetNodes();
		const size_t numNodes = level.GetNumNodes();
		for(size_t i = 0;i < numNodes; ++i)
		{
			Vector2d_s a(nodes[i].iX, nodes[i].iY);
			Vector2d_s b(nodes[i].iCX, nodes[i].iCY);
			b.Add(a);

			float x1 = (a.fX * scaleX) + offsetX;
			float y1 = (a.fY * scaleY) + offsetY;
			float x2 = (b.fX * scaleX) + offsetX;
			float y2 = (b.fY * scaleY) + offsetY;

			lineColor(screen, (int16_t)x1, (int16_t)y1, (int16_t)x2, (int16_t)y2, 0x0000FFFF);
		}
	}

	if(flags & DF_SHOW_SUBSECTORS)
	{		
		const GLSegment5_s *segments = level.GetGLSegments();
		const GLSubSector3_s *subSectors = level.GetGLSubSectors();
		const size_t numSubSectors = level.GetNumGLSubSectors();
		for(size_t i = 0;i < numSubSectors; ++i)
		{
			const GLSubSector3_s &sub = subSectors[i];
			
			for(uint32_t j = 0;j < sub.iCount; ++j)
			{							
				const GLSegment5_s &current = segments[sub.iFirstSeg+j];				

				Vector2d_s a = level.GetGLVertex(current.iStartVertex);
				Vector2d_s b = level.GetGLVertex(current.iEndVertex);				

				float x1 = (a.fX * scaleX) + offsetX;
				float y1 = (a.fY * scaleY) + offsetY;
				float x2 = (b.fX * scaleX) + offsetX;
				float y2 = (b.fY * scaleY) + offsetY;

				lineColor(screen, (int16_t)x1, (int16_t)y1, (int16_t)x2, (int16_t)y2, 0xFF00FFFF);				
			}
		}
	}

	if(flags & DF_SHOW_THINGS)
	{
		const Thing_s *things = level.GetThings();
		for(size_t i = 0, len = level.GetNumThings(); i < len; ++i)
		{
			float x1 = (things[i].iX * scaleX) + offsetX;
			float y1 = (things[i].iY * scaleY) + offsetY;

			circleColor(screen, (int16_t)x1, (int16_t)y1, 2, yellow);
		}
	}
}

void LoadLevel(WadFile_c &file, WadLevel_c &level, const char *name)
{
	std::cout << "loading " << name << std::endl;
	try
	{
		file.LoadLevel(level, name);
	}
	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

int main(int argc, char **argv)
{
	using namespace std;

	if(argc < 3)
	{
		cerr << "Insuficient parameters, usage: wadloader <wadname> <levelname>" << endl;
		
		return EXIT_FAILURE;
	}

	std::vector<const char *> levelsNames;
	for(int i =  2;i < argc; ++i)
		levelsNames.push_back(argv[i]);

	int currentLevel = 0;

	WadFile_c wad(argv[1]);
	WadLevel_c level;

	try
	{
		wad.LoadLevel(level, levelsNames[0]);
	}
	catch(std::exception &e)
	{
		cerr << "Error: " << e.what() << endl;		
		return EXIT_FAILURE;
	}

	if(SDL_Init(SDL_INIT_VIDEO))
	{
		cerr << "Cannot initialize SDL" << endl;

		return EXIT_FAILURE;
	}

	atexit(SDL_Quit);
	
	uint32_t flags = 0;
	try
	{
		SDL_Surface *screen = SDL_SetVideoMode(800, 600, 32, 0);
		if(screen == NULL)
		{
			throw std::exception("Cannot set video mode");
		}

		bool quit = false;
		while(!quit)
		{
			SDL_Event ev;
			while(SDL_PollEvent(&ev))
			{
				switch(ev.type)
				{
					case SDL_KEYDOWN:
						if(ev.key.keysym.sym == SDLK_ESCAPE)
							quit = true;

						if((ev.key.keysym.sym == SDLK_e) && (ev.key.keysym.mod & KMOD_SHIFT))
						{
							ExportAllFlats(wad);
						}
						if((ev.key.keysym.sym == SDLK_e) && !(ev.key.keysym.mod & KMOD_SHIFT))
						{
							ExportFlats(level, wad);
						}
						if(ev.key.keysym.sym == SDLK_n)
						{
							flags ^= DF_SHOW_LINE_SIDES;							
						}
						if(ev.key.keysym.sym == SDLK_s)
						{
							flags ^= DF_SHOW_SEGMENTS;
						}
						if(ev.key.keysym.sym == SDLK_p)
						{
							flags ^= DF_SHOW_PARTITION_LINES;
						}
						if(ev.key.keysym.sym == SDLK_t)
						{
							flags ^= DF_SHOW_THINGS;
						}
						if(ev.key.keysym.sym == SDLK_u)
						{
							flags ^= DF_SHOW_SUBSECTORS;
						}
						if(ev.key.keysym.sym == SDLK_v)
						{
							flags ^= DF_SHOW_VERTICES;
						}						
						
						
						if(ev.key.keysym.sym == SDLK_PAGEDOWN)
						{
							currentLevel = (currentLevel + 1) % levelsNames.size();							
							LoadLevel(wad, level, levelsNames[currentLevel]);
						}
						if(ev.key.keysym.sym == SDLK_PAGEUP)
						{
							--currentLevel;
							if(currentLevel < 0)
								currentLevel = levelsNames.size()-1;
							LoadLevel(wad, level, levelsNames[currentLevel]);
						}
						break;
					case SDL_QUIT:
						quit = true;
						break;
				}
			}

			SDL_FillRect(screen, NULL, 0);
			DrawLevel(level, screen, flags);

			SDL_Flip(screen);
		}		
	}
	catch(std::exception &e)
	{
		cerr << "Error: " << e.what() << endl;		
	}
	exit(0);

	return 0;
}
