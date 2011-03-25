#ifndef WADLEVEL_H
#define WADLEVEL_H

#include <vector>

#include "Vector2d.h"
#include "WadTypes.h"

class WadFile_c;
struct Directory_s;

class WadLevel_c
{
	public:
		void Load(WadFile_c &file, const Directory_s *levelDir);

		inline const Vertex_s &GetMin() const;
		inline const Vertex_s &GetMax() const;

		inline const LineDef_s *GetLineDefs() const;
		inline const size_t GetNumLineDefs() const;

		inline const Vertex_s *GetVertices() const;

		inline const Segment_s *GetSegments() const;
		inline const size_t GetNumSegments() const;

		inline const Node_s *GetNodes() const;
		inline const size_t GetNumNodes() const;

		inline const GLSubSector3_s *GetGLSubSectors() const;
		inline const size_t GetNumGLSubSectors() const;

		inline const GLSegment5_s *GetGLSegments() const;

		inline Vector2d_s GetGLVertex(int glIndex) const;

		inline const Thing_s *GetThings() const;
		inline const size_t GetNumThings() const;

	private:
		template<typename T>
		void LoadLump(std::vector<T> &dest, LevelLumps_e lump, WadFile_c &file, const Directory_s *levelDir, const char *szMagic = NULL);		

	private:
		std::vector<LineDef_s> vecLineDefs;
		std::vector<Vertex_s> vecVertices;
		std::vector<Segment_s> vecSegments;
		std::vector<Node_s> vecNodes;
		std::vector<SubSector_s> vecSubSectors;
		std::vector<GLVertex_s> vecGLVertices;
		std::vector<GLSegment5_s> vecGLSegments;
		std::vector<GLSubSector3_s> vecGLSubSectors;	

		std::vector<Thing_s> vecThings;

		Vertex_s stMin;
		Vertex_s stMax;		
};

inline const Vertex_s &WadLevel_c::GetMin() const
{
	return stMin;
}

inline const Vertex_s &WadLevel_c::GetMax() const
{
	return stMax;
}

inline const LineDef_s *WadLevel_c::GetLineDefs() const
{
	return &vecLineDefs[0];
}
inline const size_t WadLevel_c::GetNumLineDefs() const
{
	return vecLineDefs.size();
}

inline const Vertex_s *WadLevel_c::GetVertices() const
{
	return &vecVertices[0];
}

inline const Segment_s *WadLevel_c::GetSegments() const
{
	return &vecSegments[0];
}

inline const size_t WadLevel_c::GetNumSegments() const
{
	return vecSegments.size();
}

inline const Node_s *WadLevel_c::GetNodes() const
{
	return &vecNodes[0];
}
		
inline const size_t WadLevel_c::GetNumNodes() const
{
	return vecNodes.size();
}

inline const GLSubSector3_s *WadLevel_c::GetGLSubSectors() const
{
	return &vecGLSubSectors[0];
}

inline const size_t WadLevel_c::GetNumGLSubSectors() const
{
	return vecGLSubSectors.size();
}

inline const GLSegment5_s *WadLevel_c::GetGLSegments() const
{
	return &vecGLSegments[0];
}

inline const Thing_s *WadLevel_c::GetThings() const
{
	return &vecThings[0];
}

inline const size_t WadLevel_c::GetNumThings() const
{
	return vecThings.size();
}

#define FIX_VERTEX_INDEX(X) (X & ~((1 << 31) | (1 << 30)))

inline Vector2d_s WadLevel_c::GetGLVertex(int glIndex) const
{
	if(glIndex & VERT_IS_GL)
	{
		return Vector2d_s(
			static_cast<float>(vecGLVertices[FIX_VERTEX_INDEX(glIndex)].iX) / 65536.0f,
			static_cast<float>(vecGLVertices[FIX_VERTEX_INDEX(glIndex)].iY) / 65536.0f
		);
	}
	else
	{
		return Vector2d_s(
			static_cast<float>(vecVertices[glIndex].iX),
			static_cast<float>(vecVertices[glIndex].iY)
		);
	}
}

#endif
