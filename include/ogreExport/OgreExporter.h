#ifndef OGRE_EXPORTER_H
#define OGRE_EXPORTER_H

#include <set>
#include <string>

#include "WadTypes.h"

class SDLTexture_c;
class WadLevel_c;
class WadFile_c;

class OgreExporter_c
{
	public:
		struct FlatNameFunctor_s
		{
			bool operator()(const Name_u &lhs, const Name_u &rhs)const
			{
				return lhs.uName < rhs.uName;
			}
		};
		typedef std::set<Name_u, FlatNameFunctor_s> NameSet_t;

	public:
		OgreExporter_c(const char *outputDirectory);

		void ExportLevel(const WadLevel_c &level, const WadFile_c &file);

	private:
		void CreateDirectories();

		void ExportLevelMaterials(const WadLevel_c &level, const WadFile_c &file);

		void SaveTexture(SDLTexture_c &texture, Name_u name);

		void ExportWall(SDLTexture_c &texture,  const WadFile_c &file, Name_u name);
		void ExportWalls(const WadLevel_c &level, const WadFile_c &file);

		void CreateResourcesCfg();

	private:
		std::string strOutputDirectory;

		std::string strLevelDirectory;
		std::string strMaterialScriptsDirectory;
		std::string strMaterialTexturesDirectory;

		NameSet_t setExportedTextures;
};

#endif