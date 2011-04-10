#include "WadPatch.h"

#include "WadFile.h"

void WadPatch_c::Load(WadFile_c &file, const Directory_s &patchDir)
{
	const size_t headerSize = sizeof(stHeader);

	vecData.resize(patchDir.iSize - headerSize);
	file.ReadLump(reinterpret_cast<char *>(&stHeader), headerSize, reinterpret_cast<char *>(&vecData[0]), patchDir);

	piOffset = reinterpret_cast<uint32_t *>(&vecData[0]);
	puPixels = &vecData[stHeader.iWidth * 4];
}