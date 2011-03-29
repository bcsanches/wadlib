#include "WadFile.h"

#include <sstream>

#include "ITexture.h"
#include "WadLevel.h"

WadFile_c::WadFile_c(const char *fileName):
	clFile(fileName, std::ios_base::binary | std::ios_base::in)
{
	if(!clFile.is_open())
		throw std::exception("Cannot open file");

	char id[4];
	clFile.read(id, 4);

	if(strncmp(id, "PWAD", 4) == 0)
		eType = WT_PATCH;
	else if(strncmp(id, "IWAD", 4) ==0)
		eType = WT_REGULAR;
	else
		throw std::exception("File is not WAD");

	clFile.read(reinterpret_cast<char *>(&stLumpInfo), sizeof(stLumpInfo));

	//Read the directory table
	clFile.seekg(stLumpInfo.iOffset);
	vecDirectories.resize(stLumpInfo.iNum);
	clFile.read(reinterpret_cast<char *>(&vecDirectories[0]), sizeof(Directory_s) * stLumpInfo.iNum);

	pstFStart = this->FindLump("F_START");
	pstFEnd = this->FindLump("F_END");

	this->ReadRawLump(vecColorMap, "COLORMAP");
	this->ReadRawLump(vecPalette, "PLAYPAL");	
}

void WadFile_c::ReadRawLump(std::vector<uint8_t> &dest, const char *szName)
{
	const Directory_s *directory = this->FindLump(szName);	

	dest.resize(directory->iSize);
	this->ReadLump(reinterpret_cast<char *>(&dest[0]), *directory, NULL);
}

const Directory_s *WadFile_c::FindLump(const char *name) const
{
	for(size_t i = 0, len = vecDirectories.size();i < len; ++i)
	{
		if(strncmp(vecDirectories[i].archName, name, 8) == 0)
			return &vecDirectories[i];
	}
	
	std::stringstream stream;
	stream << "Lump " << std::string(name, 8) << " not found.";
		
	throw std::exception(stream.str().c_str());	
}

const Directory_s *WadFile_c::FindLump(const char *name, const Directory_s *begin, const Directory_s *end) const
{
	for(;begin != end; ++begin)
	{
		if(strncmp(begin->archName, name, 8) == 0)
			return begin;
	}

	std::stringstream stream;
	stream << "Lump " << std::string(name, 8) << " not found.";
		
	throw std::exception(stream.str().c_str());	
}

void WadFile_c::LoadLevel(WadLevel_c &level, const char *name)
{	
	const Directory_s *dir = this->FindLump(name);		
	
	level.Load(*this, dir, (&vecDirectories.back() - dir)+1);
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
			throw std::exception(stream.str().c_str());				
		}

		magicSize = strlen(szMagic);
	}

	clFile.read(dest, dir.iSize  - magicSize);
}

void WadFile_c::LoadFlat(ITexture_c &texture, const char *pchName)
{
	const Directory_s *flatDirectory = this->FindLump(pchName, pstFStart+1, pstFEnd);	

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
