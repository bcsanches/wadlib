#ifndef WAD_TYPES_H
#define WAD_TYPES_H

typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

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

#endif