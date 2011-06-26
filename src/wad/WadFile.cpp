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


#include "WadFile.h"

#include <sstream>

#include "ITexture.h"
#include "WadLevel.h"

WadFile_c::WadFile_c(const char *fileName):
	clFile(fileName, std::ios_base::binary | std::ios_base::in)
{
	if(!clFile.is_open())
		throw std::runtime_error("Cannot open file");

	char id[4];
	clFile.read(id, 4);

	if(strncmp(id, "PWAD", 4) == 0)
		eType = WT_PATCH;
	else if(strncmp(id, "IWAD", 4) ==0)
		eType = WT_REGULAR;
	else
		throw std::runtime_error("File is not WAD");

	clFile.read(reinterpret_cast<char *>(&stLumpInfo), sizeof(stLumpInfo));

	//Read the directory table
	clFile.seekg(stLumpInfo.iOffset);
	vecDirectories.resize(stLumpInfo.iNum);
	clFile.read(reinterpret_cast<char *>(&vecDirectories[0]), sizeof(Directory_s) * stLumpInfo.iNum);

	pstFStart = this->FindLump("F_START");
	pstFEnd = this->FindLump("F_END");

	this->ReadRawLump(vecColorMap, "COLORMAP");
	this->ReadRawLump(vecPalette, "PLAYPAL");	

	const Directory_s *textures = this->FindLump("TEXTURE1");
	this->LoadTexturesData(*textures);
	if((textures = this->TryFindLump("TEXTURE2")) != NULL)
		this->LoadTexturesData(*textures);

	pstPatchesStart = this->FindLump("P_START");
	pstPatchesEnd = this->FindLump("P_END");

	this->LoadPatchLumpsNames();
}

void WadFile_c::LoadTexturesData(const Directory_s &textures)
{
	clFile.seekg(textures.iOffset);

	size_t numTextures;
	clFile.read(reinterpret_cast<char *>(&numTextures), sizeof(numTextures));

	//skip textures offsets
	clFile.seekg(numTextures * 4, std::ios_base::cur);

	size_t pos = vecTextures.size();
	vecTextures.resize(pos + numTextures);

	for(size_t i = 0; i < numTextures; ++i, ++pos)
	{
		clFile.read(reinterpret_cast<char *>(&vecTextures[pos]), sizeof(Texture_s));

		size_t patchPosition = vecTexturePatches.size();
		vecTexturePatches.resize(patchPosition + vecTextures[pos].uPatchCount);
		clFile.read(reinterpret_cast<char *>(&vecTexturePatches[patchPosition]), sizeof(TexturePatch_s) * vecTextures[pos].uPatchCount);

		vecTexturesPatchesIndex.push_back(patchPosition);
	}
}

void WadFile_c::LoadPatchLumpsNames()
{
	const Directory_s *directory = this->FindLump("PNAMES");

	size_t headerSize = sizeof(uint32_t);
	vecPatchLumps.resize((directory->iSize - headerSize) / sizeof(vecPatchLumps[0]));

	uint32_t numPatches;
	this->ReadLump(reinterpret_cast<char *>(&numPatches), headerSize, reinterpret_cast<char *>(&vecPatchLumps[0]), *directory);
}

void WadFile_c::ReadRawLump(std::vector<uint8_t> &dest, const char *szName)
{
	const Directory_s *directory = this->FindLump(szName);	

	dest.resize(directory->iSize);
	this->ReadLump(reinterpret_cast<char *>(&dest[0]), *directory, NULL);
}

const Directory_s *WadFile_c::TryFindLump(Name_u name) const
{
	for(size_t i = 0, len = vecDirectories.size();i < len; ++i)
	{
		//if(strncmp(vecDirectories[i].unName.archName, name, 8) == 0)
		if(vecDirectories[i].unName.uName == name.uName)
			return &vecDirectories[i];
	}

	return NULL;	
}

const Directory_s *WadFile_c::FindLump(Name_u name) const
{
	const Directory_s *dir = this->TryFindLump(name);
	if(dir == NULL)
	{
		std::stringstream stream;
		stream << "Lump " << std::string(name.archName, 8) << " not found.";
			
		throw std::runtime_error(stream.str().c_str());	
	}

	return dir;
}

const Directory_s *WadFile_c::FindLump(Name_u name, const Directory_s *begin, const Directory_s *end) const
{	
	for(;begin != end; ++begin)
	{
		//if(strncmp(begin->unName.archName, name, 8) == 0)
		if(begin->unName.uName == name.uName)
			return begin;
	}

	std::stringstream stream;
	stream << "Lump " << std::string(name.archName, 8) << " not found.";
		
	throw std::runtime_error(stream.str());	
}

void WadFile_c::LoadLevel(WadLevel_c &level, const char *name)
{	
	const Directory_s *dir = this->FindLump(name);		
	
	level.Load(*this, dir, (&vecDirectories.back() - dir)+1);
}

void WadFile_c::ReadLump(char *header, size_t headerSize, char *data, const Directory_s &dir)
{
	clFile.seekg(dir.iOffset);
	clFile.read(header, headerSize);
	clFile.read(data, dir.iSize - headerSize);
}

void WadFile_c::ReadLump(char *dest, const Directory_s &dir, const char *szMagic)
{
	clFile.seekg(dir.iOffset);

	size_t magicSize = 0;
	if(szMagic != NULL)
	{
		char szMagicCheck[4];
		clFile.read(szMagicCheck, 4);
		if(strncmp(szMagicCheck, szMagic, 4))
		{
			std::stringstream stream;
			stream << "Magic " << szMagic << " failed." << std::endl;
			throw std::runtime_error(stream.str());				
		}

		magicSize = strlen(szMagic);
	}

	clFile.read(dest, dir.iSize  - magicSize);
}

void WadFile_c::LoadFlat(ITexture_c &texture, Name_u name)
{
	const Directory_s *flatDirectory = this->FindLump(name, pstFStart+1, pstFEnd);	

	texture.SetSize(64, 64);

	uint8_t *pixels = reinterpret_cast<uint8_t*>(texture.GetPixels());

	this->ReadLump(reinterpret_cast<char *>(texture.GetPixels()), *flatDirectory, NULL);

	for(size_t sz = 0;sz < 64*64; ++sz)
	{
		pixels[sz] = vecColorMap[pixels[sz] + (256 * 0)];
	}

	texture.SetPalette(&vecPalette[768*0]);
}

const Directory_s *WadFile_c::FlatBegin()
{
	return pstFStart + 1;
}

const Directory_s *WadFile_c::FlatEnd()
{
	return pstFEnd;
}

void WadFile_c::LoadTexture(ITexture_c &texture, uint32_t index)
{
	Texture_s &textureInfo = vecTextures[index];

	//texture.SetSize(textureInfo.uHeight, textureInfo.uWidth);	
	texture.SetSize(textureInfo.uWidth, textureInfo.uHeight);	

	std::size_t pixelCount = 0;
	for(int i = 0;i < textureInfo.uPatchCount; ++i)
	{
		TexturePatch_s &texPatch = vecTexturePatches[vecTexturesPatchesIndex[index] + i];

		const WadPatch_c &realPatch = this->GetPatch(vecPatchLumps[texPatch.uPatch]);
		
		uint8_t *texPixels = reinterpret_cast<uint8_t *>(texture.GetPixels());						

		int x1 = texPatch.iOriginX;
		int x2 = x1 + realPatch.GetWidth();

		int x = std::max(x1, 0);

		x2 = std::min(x2, static_cast<int>(textureInfo.uWidth));

		size_t patchTotal = 0;
		for(;x < x2; ++x)
		{
			const uint8_t *patchPixels = realPatch.GetColumn(x - x1);
			//uint8_t *destColumn = texPixels + (x * textureInfo.uHeight);
			size_t destColumnOffset = 0;

			for(;;)
			{
				uint8_t topDelta = *patchPixels;
				if(topDelta == 0xff)
					break;

				if(topDelta > 0)
				{
					//destColumn = texPixels + (x * textureInfo.uHeight);
					destColumnOffset = 0;
				}

				int patchLength = *(patchPixels+1);
				int count = patchLength;

				int position = texPatch.iOriginY + topDelta;
				if(position < 0)
				{
					count += position;
					position = 0;
				}

				if(position + count > textureInfo.uHeight)
				{
					count = textureInfo.uHeight - position;
				}											

				if(count > 0)
				{
					const uint8_t *source = patchPixels + 3;

					//memcpy(destColumn + position, source, count);
					for(int pixelCounter = 0;pixelCounter < count; ++pixelCounter)
					{
						texPixels[((destColumnOffset + pixelCounter + position) * textureInfo.uWidth) + x] = *(source + pixelCounter);
					}
					//destColumn += count;
					destColumnOffset += count;
				}
							
				patchPixels += patchLength + 4;						
			}		
		}
	}

	texture.SetPalette(&vecPalette[768*0]);
}

size_t WadFile_c::GetNumTextures() const
{
	return vecTextures.size();
}

Name_u WadFile_c::GetTextureName(uint32_t index) const
{
	return vecTextures[index].unName;
}

uint32_t WadFile_c::FindTextureIndex(Name_u name) const
{
	for(size_t i = 0, len = vecTextures.size();i < len; ++i)
	{
		if(vecTextures[i].unName.uName == name.uName)
			return i;
	}

	std::stringstream stream;
	stream << "Cannot find texture " << std::string(name.archName, 8);	
	throw std::runtime_error(stream.str());
}

const WadPatch_c &WadFile_c::GetPatch(const Name_u &name)
{
	WadPatchMap_t::iterator it = mapWadPatches.lower_bound(name.uName);
	if((it != mapWadPatches.end()) && (!mapWadPatches.key_comp()(name.uName, it->first)))
		return it->second;
	else
	{
		const Directory_s *patchLump = this->FindLump(name.archName);

		WadPatch_c &patch = mapWadPatches.insert(it, std::make_pair(name.uName, WadPatch_c()))->second;

		patch.Load(*this, *patchLump);

		return patch;
	}
}