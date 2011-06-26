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


#ifndef WAD_FILE_H
#define WAD_FILE_H

#include <fstream>
#include <map>
#include <vector>

#include "WadPatch.h"
#include "WadTypes.h"

enum WadTypes_e
{
	WT_PATCH,
	WT_REGULAR
};

class ITexture_c;
class WadLevel_c;

/*
http://doom.wikia.com/wiki/WAD
http://tkboom.sourceforge.net/wadfile_spec.shtml

*/
class WadFile_c
{
	public:
		WadFile_c(const char *fileName);

		void LoadLevel(WadLevel_c &level, const char *name);
		void ReadLump(char *dest, const Directory_s &dir, const char *szMagic); 
		void ReadLump(char *header, size_t headerSize, char *data, const Directory_s &dir);
		
		void LoadFlat(ITexture_c &texture, Name_u name);
		void LoadTexture(ITexture_c &texture, uint32_t index);
		
		Name_u GetTextureName(uint32_t index) const;
		size_t GetNumTextures() const;

		uint32_t FindTextureIndex(Name_u name) const;

		const Directory_s *FlatBegin();
		const Directory_s *FlatEnd();		

	private:		
		const Directory_s *FindLump(Name_u name, const Directory_s *begin, const Directory_s *end) const;
		const Directory_s *FindLump(Name_u name) const;
		const Directory_s *TryFindLump(Name_u name) const;

		void ReadRawLump(std::vector<uint8_t> &dest, const char *szName);

		void LoadTexturesData(const Directory_s &textures);
		void LoadPatchLumpsNames();

		const WadPatch_c &GetPatch(const Name_u &name);

	private:
		std::ifstream clFile;

		WadTypes_e eType;		

		LumpInfo_s stLumpInfo;

		std::vector<Directory_s> vecDirectories;
		std::vector<uint8_t> vecColorMap;
		std::vector<uint8_t> vecPalette;

		//Flats
		const Directory_s *pstFStart;
		const Directory_s *pstFEnd;

		const Directory_s *pstPatchesStart;
		const Directory_s *pstPatchesEnd;		

		std::vector<Texture_s> vecTextures;
		std::vector<int> vecTexturesPatchesIndex;
		std::vector<TexturePatch_s> vecTexturePatches;

		//The name of the patch lumps, loaded from PNAMES lump
		std::vector<Name_u> vecPatchLumps;

		typedef std::map<uint64_t, WadPatch_c> WadPatchMap_t;
		WadPatchMap_t mapWadPatches;
};

#endif
