#include "WadLevel.h"

#include "WadFile.h"
#include "Vector2d.h"

#include <sstream>

//Same order as LevelLumps_e
static const char *pszLumpsNames_gl[] =
{
	"NAME",
	"THINGS",
	"LINEDEFS",
	"SIDEDEFS",
	"VERTEXES",
	"SEGS",
	"SSECTORS",
	"NODES",
	"SECTORS",
	"REJECT",
	"BLOCKMAP",
	"GLNAME",
	"GL_VERT",
	"GL_SEGS",
	"GL_SSECT",
	"GL_NODES",
	"GL_PVS"
};


template<typename T>
void WadLevel_c::LoadLump(std::vector<T> &dest, LevelLumps_e lump, WadFile_c &file, const Directory_s *levelDir, const char *szMagic)
{
	if(strncmp(levelDir[lump].archName, pszLumpsNames_gl[lump], 8))
	{
		std::stringstream stream;
		stream << pszLumpsNames_gl[lump] << " not found";
		throw std::exception(stream.str().c_str());
	}

	size_t magicSize = szMagic ? strlen(szMagic) : 0;
	dest.resize((levelDir[lump].iSize - magicSize)/ sizeof(T));
	file.ReadLump(reinterpret_cast<char *>(&dest[0]), levelDir[lump], szMagic);
}

void WadLevel_c::Load(WadFile_c &file, const Directory_s *levelDir)
{
	LoadLump(vecLineDefs, LL_LINEDEFS, file, levelDir);
	LoadLump(vecVertices, LL_VERTICES, file, levelDir);	
	LoadLump(vecSegments, LL_SEGS, file, levelDir);	
	LoadLump(vecNodes, LL_NODES, file, levelDir);	
	LoadLump(vecSubSectors, LL_SSECTORS, file, levelDir);	
	LoadLump(vecGLVertices, LL_GL_VERT, file, levelDir, "gNd5");
	LoadLump(vecGLSegments, LL_GL_SEGS, file, levelDir);
	LoadLump(vecGLSubSectors, LL_GL_SSECTORS, file, levelDir);

	stMin.iX = 32767;
	stMax.iX = -32768;
	stMin.iY = 32767;
	stMax.iY = -32768;

	for(size_t i = 0;i < vecVertices.size(); ++i)
	{
		stMin.iX = std::min(vecVertices[i].iX, stMin.iX);
		stMin.iY = std::min(vecVertices[i].iY, stMin.iY);
		stMax.iX = std::max(vecVertices[i].iX, stMax.iX);
		stMax.iY = std::max(vecVertices[i].iY, stMax.iY);
	}	
}
