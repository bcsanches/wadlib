#ifndef WAD_FILE_H
#define WAD_FILE_H

#include <fstream>
#include <SDL.h>

#include <vector>

enum WadTypes_e
{
	WT_PATCH,
	WT_REGULAR
};

struct LumpInfo_s
{
	int32_t iNum;
	int32_t iOffset;
};

struct Directory_s
{
	//Offset of the lump
	int32_t iOffset;

	//lump size
	int32_t iSize;

	//name of the lump
	char	archName[8];
};

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

	private:
		Directory_s *FindLump(const char *name, size_t startIndex = 0);

	private:
		std::ifstream clFile;

		WadTypes_e eType;		

		LumpInfo_s stLumpInfo;

		std::vector<Directory_s> vecDirectories;
};

#endif
