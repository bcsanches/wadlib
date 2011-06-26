/*
wadlib
Copyright (c) 2011 Bruno Sanches  http://code.google.com/p/wadlib

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:


3. This notice may not be removed or altered from any source distribution.
*/


#ifndef WAD_TYPES_H
#define WAD_TYPES_H


typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

#ifndef strncpy
#include <string.h>
#endif

union Name_u
{
	Name_u():
		uName(0)
	{
	}

	Name_u(const char *archName)
	{
		uName = 0;
		strncpy(this->archName, archName, 8);
	}

	#pragma pack(push)
	#pragma pack(1)
	//name of the lump
	char	 archName[8];
	uint64_t uName;
	#pragma pack(pop)
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
	
	Name_u unName;	
};

struct Thing_s
{
	int16_t iX;
	int16_t iY;
	int16_t iAngle;
	int16_t iType;
	int16_t iFlags;
};

struct LineDef_s
{
	int16_t iStartVertex;
	int16_t iEndVertex;
	int16_t iFlags;
	int16_t iSpecialType;
	int16_t iSectorTag;
	int16_t iRightSideDef;
	int16_t iLeftSideDef;
};

struct Vertex_s
{
	int16_t iX;
	int16_t iY;
};

struct GLVertex_s
{
	int32_t iX;
	int32_t iY;
};

struct Segment_s
{
	int16_t iStartVertex;
	int16_t iEndVertex;
	int16_t iAngle;
	int16_t iLineDef;

	//0 (same as linedef) or 1 (opposite of linedef)
	int16_t iDirection;

	//distance along linedef to start of seg
	int16_t iOffset;

	inline int GetDirectionStartVertex() const
	{
		return iDirection ? iEndVertex : iStartVertex;
	}

	inline int GetDirectionEndVertex() const
	{
		return iDirection ? iStartVertex : iEndVertex;
	}
};

struct GLSegment5_s
{
	uint32_t iStartVertex;
	uint32_t iEndVertex;
	uint16_t iLineDef;
	uint16_t iSide;
	uint32_t iPartnerSegment;
};

#define VERT_IS_GL (1 << 31)

struct Node_s
{
	//Partition line start
	int16_t iX;
	int16_t iY;

	//Partition line change
	int16_t iCX;
	int16_t iCY;

	int16_t ariRightBBox[4];
	int16_t ariLeftBBox[4];

	int16_t iRightChild;
	int16_t iLeftChild;
};

struct SubSector_s
{
	int16_t iCount;
	int16_t iFirstSeg;
};

struct GLSubSector3_s
{
   uint32_t iCount;
   uint32_t iFirstSeg;
};


struct Sector_s
{
	int16_t iFloorHeight;
	int16_t iCeilHeight;
	
	Name_u unFloorTexture;
	Name_u unCeilTexture;	

	int16_t iLightLevel;
	uint16_t uType;
	uint16_t uTag;
};


enum LevelLumps_e
{
  LL_NAME,
  LL_THINGS,
  LL_LINEDEFS,
  LL_SIDEDEFS,
  LL_VERTICES,
  LL_SEGS,
  LL_SSECTORS,
  LL_NODES,
  LL_SECTORS,
  LL_REJECT,
  LL_BLOCKMAP,
  LL_GL_NAME,
  LL_GL_VERT,
  LL_GL_SEGS,
  LL_GL_SSECTORS,
  LL_GL_NODES,
  LL_GL_PVS
};

#pragma pack(push)
#pragma pack(1)
struct Texture_s
{
	Name_u unName;

	uint32_t uMasked;
	uint16_t uWidth;
	uint16_t uHeight;

	//obsolete, ignore
	uint32_t uColumnDirectory;

	uint16_t uPatchCount;
};
#pragma pack(pop)

struct TexturePatch_s
{
	int16_t iOriginX;
	int16_t iOriginY;

	uint16_t uPatch;

	int16_t iStepDir;
	uint16_t uColormap;
};

struct Patch_s
{ 
    int16_t iWidth;
    int16_t iHeight; 
    int16_t iLeftOffset;
    int16_t iTopOffset;        
};

struct SideDef_s
{
	int16_t iOffsetX;
	int16_t iOffsety;

	Name_u uUpperTexture;
	Name_u uLowerTexture;
	Name_u uMiddleTexture;

	int16_t iSector;
};

#endif
