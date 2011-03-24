#include "WadFile.h"

#include <sstream>

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
}

Directory_s *WadFile_c::FindLump(const char *name, size_t startIndex)
{
	for(size_t i = startIndex;i < vecDirectories.size(); ++i)
	{
		if(strncmp(vecDirectories[i].archName, name, 8) == 0)
		{
			return &vecDirectories[i];			
		}
	}

	return NULL;
}

void WadFile_c::LoadLevel(WadLevel_c &level, const char *name)
{	
	Directory_s *levelLump = this->FindLump(name);
	if(levelLump == NULL)
	{
		std::stringstream stream;
		stream << "Level " << name << " not found";
		throw std::exception(stream.str().c_str());
	}	
	
	level.Load(*this, levelLump);		
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
