#ifndef WAD_PATCH_H
#define WAD_PATCH_H

#include "WadTypes.h"

#include <vector>
#include <assert.h>

class WadFile_c;

class WadPatch_c
{
	public:
		void Load(WadFile_c &file, const Directory_s &patchDir);
		
		inline int GetWidth() const;

		inline const uint8_t *GetPixels() const;
		inline const uint8_t *GetColumn(size_t column) const;

	private:		
		Patch_s stHeader;

		std::vector<uint8_t> vecData;

		const uint32_t *piOffset;
		const uint8_t *puPixels;
};

inline int WadPatch_c::GetWidth() const
{
	return stHeader.iWidth;
}		

inline const uint8_t *WadPatch_c::GetPixels() const
{
	return puPixels;
}

inline const uint8_t *WadPatch_c::GetColumn(size_t column) const
{
	assert(column < (size_t)stHeader.iWidth);

	return &vecData[piOffset[column] - sizeof(stHeader)];
}

#endif