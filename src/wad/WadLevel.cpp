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


#include "WadLevel.h"

#include "WadFile.h"
#include "Vector2d.h"

#include <algorithm>
#include <sstream>
#include <cstring>
#include <stdexcept>

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
	if(strncmp(levelDir[lump].unName.archName, pszLumpsNames_gl[lump], 8))
	{
		std::stringstream stream;
		stream << pszLumpsNames_gl[lump] << " not found";
		throw std::runtime_error(stream.str().c_str());
	}

	size_t magicSize = szMagic ? strlen(szMagic) : 0;
	dest.resize((levelDir[lump].iSize - magicSize)/ sizeof(T));
	file.ReadLump(reinterpret_cast<char *>(&dest[0]), levelDir[lump], szMagic);
}

void WadLevel_c::Load(WadFile_c &file, const Directory_s *levelDir, size_t numDirectories)
{	
	strName.assign(levelDir->unName.archName, 8);

	LoadLump(vecThings, LL_THINGS, file, levelDir);
	LoadLump(vecLineDefs, LL_LINEDEFS, file, levelDir);
	LoadLump(vecSideDefs, LL_SIDEDEFS, file, levelDir);
	LoadLump(vecVertices, LL_VERTICES, file, levelDir);
	LoadLump(vecSegments, LL_SEGS, file, levelDir);
	LoadLump(vecSubSectors, LL_SSECTORS, file, levelDir);
	LoadLump(vecNodes, LL_NODES, file, levelDir);
	LoadLump(vecSectors, LL_SECTORS, file, levelDir);

	//Do we have GL data?
	if((LL_GL_NAME < numDirectories) && (strncmp(levelDir[LL_GL_NAME].unName.archName, pszLumpsNames_gl[LL_GL_NAME], 8) == 0))
	{
		LoadLump(vecGLVertices, LL_GL_VERT, file, levelDir, "gNd5");
		LoadLump(vecGLSegments, LL_GL_SEGS, file, levelDir);
		LoadLump(vecGLSubSectors, LL_GL_SSECTORS, file, levelDir);
	}

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
