#ifndef OGRE_EXPORTER_H
#define OGRE_EXPORTER_H

#include <string>

class WadLevel_c;
class WadFile_c;

class OgreExporter_c
{
	public:
		OgreExporter_c(const char *outputDirectory);

		void ExportLevel(const WadLevel_c &level, const WadFile_c &file);

	private:
		void CreateDirectories();

	private:
		std::string strOutputDirectory;

		std::string strLevelDirectory;
		std::string strMaterialScriptsDirectory;
		std::string strMaterialTexturesDirectory;

};

#endif