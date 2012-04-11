#include "OgreExporter.h"

#include <OgreDefaultHardwareBufferManager.h>
#include <OgreLodStrategyManager.h>
#include <OgreLogManager.h>
#include <OgreManualObject.h>
#include <OgreMeshManager.h>
#include <OgreMeshSerializer.h>
#include <OgreResourceGroupManager.h>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include "SDLTexture.h"
#include "WadFile.h"
#include "WadLevel.h"

namespace fs = boost::filesystem;

OgreExporter_c::OgreExporter_c(const char *outputDirectory):	
	strOutputDirectory(outputDirectory)
{
	this->CreateDirectories();
}

void OgreExporter_c::CreateDirectories()
{	
	fs::path path(strOutputDirectory);

	fs::create_directories(path);

	fs::path subPath = path / "levels";
	fs::create_directories(subPath);

	strLevelDirectory = subPath.string();

	subPath = path / "materials/scripts";
	fs::create_directories(subPath);

	strMaterialScriptsDirectory = subPath.string();

	subPath = path / "materials/textures";
	fs::create_directories(subPath);

	strMaterialTexturesDirectory = subPath.string();
}

void OgreExporter_c::ExportLevel(const WadLevel_c &level, const WadFile_c &file)
{		
	Ogre::DefaultHardwareBufferManager hardwareBufferManager;
	Ogre::ManualObject manualMesh(level.GetName());	

	const LineDef_s *lineDefs = level.GetLineDefs();
	const size_t numLineDefs = level.GetNumLineDefs();

	const SideDef_s *sideDefs = level.GetSideDefs();
	const Sector_s *sectors = level.GetSectors();

	const Vertex_s *vertices = level.GetVertices();
	for(size_t i = 0;i < numLineDefs; ++i)
	{
		if(lineDefs[i].iLeftSideDef >= 0)
			continue;

		const SideDef_s &rightSide = sideDefs[lineDefs[i].iRightSideDef];
		const Sector_s &sector = sectors[rightSide.iSector];

		std::stringstream stream ;
		stream << rightSide.uMiddleTexture.ToString();
		manualMesh.begin(stream.str(), Ogre::RenderOperation::OT_TRIANGLE_LIST);

		const Vertex_s &startVertex = vertices[lineDefs[i].iStartVertex];
		const Vertex_s &endVertex = vertices[lineDefs[i].iEndVertex];

		manualMesh.position(startVertex.iX, startVertex.iY, sector.iFloorHeight);
		manualMesh.textureCoord(0, 0);

		manualMesh.position(startVertex.iX, startVertex.iY, sector.iCeilHeight);
		manualMesh.textureCoord(0, 1);
		
		manualMesh.position(endVertex.iX, endVertex.iY, sector.iCeilHeight);
		manualMesh.textureCoord(1, 1);		

		manualMesh.position(endVertex.iX, endVertex.iY, sector.iFloorHeight);
		manualMesh.textureCoord(1, 0);	

		manualMesh.triangle(2, 1, 0);
		manualMesh.triangle(0, 3, 2);

		manualMesh.end();
	}

	namespace fs = boost::filesystem;

	fs::path path(strLevelDirectory);

	std::string levelName = level.GetName();
	levelName += ".mesh";

	path /= levelName;	

	Ogre::LogManager logManager;
	logManager.createLog("ogre.log", true, true, false);

	Ogre::ResourceGroupManager resourceGroupManager;
	Ogre::MeshManager meshManager;
	Ogre::LodStrategyManager logStrategyManager;
	Ogre::MeshPtr mesh = manualMesh.convertToMesh(level.GetName());

	Ogre::MeshSerializer serializer;
	serializer.exportMesh(mesh.get(), path.string());

	mesh.setNull();

	resourceGroupManager.shutdownAll();

	this->ExportLevelMaterials(level, file);

	this->CreateResourcesCfg();
}

void OgreExporter_c::SaveTexture(SDLTexture_c &texture, Name_u name)
{
	fs::path path(strMaterialTexturesDirectory);

	char szFileName[13] = {0};
	memcpy(szFileName, name.archName, 8);
	strcat(szFileName, ".bmp");

	path /= szFileName;
		
	texture.Save(path.string().c_str());
}

void OgreExporter_c::ExportWall(SDLTexture_c &texture, const WadFile_c &file, Name_u name)
{
	NameSet_t::iterator it = setExportedTextures.lower_bound(name);
	if((it != setExportedTextures.end()) && (!setExportedTextures.key_comp()(name, *it)))
	{
		//already exists, ignore
	}
	else
	{
		//store if first, if an error happens, we do not keep reporting it
		setExportedTextures.insert(it, name);		
		try
		{
			file.LoadTexture(texture, file.FindTextureIndex(name));
		}
		catch(std::exception &e)
		{
			std::string name(name.archName, 8);
			std::cout << "Error exporting " << name.c_str() << ": " << e.what() << std::endl;
			return;
		}		

		SaveTexture(texture, name);		
	}
}

void OgreExporter_c::ExportWalls(const WadLevel_c &level, const WadFile_c &file)
{
	std::cout << "Exporting current level walls:" << std::endl;
	
	SDLTexture_c texture;

	const SideDef_s *sides = level.GetSideDefs();
	for(size_t i = 0;i < level.GetNumSideDefs(); ++i)
	{
		const SideDef_s *side = sides+i;

		if(side->uLowerTexture.archName[0] != '-')
			ExportWall(texture, file, side->uLowerTexture);

		if(side->uMiddleTexture.archName[0] != '-')
			ExportWall(texture, file, side->uMiddleTexture);

		if(side->uUpperTexture.archName[0] != '-')
			ExportWall(texture, file, side->uUpperTexture);
	}

	std::cout << "Done." << std::endl;
}


void OgreExporter_c::ExportLevelMaterials(const WadLevel_c &level, const WadFile_c &file)
{
	this->ExportWalls(level, file);

	fs::path materialPath(strMaterialScriptsDirectory);
	materialPath /= fs::path(file.GetFileName()).filename();

	std::string finalFileName = materialPath.string();
	finalFileName += ".material";

	std::ofstream materialFile(finalFileName.c_str());

	BOOST_FOREACH(Name_u name, setExportedTextures)
	{
		materialFile << "material " << name.ToString() << "\n";
		materialFile << "{\n";
			materialFile << "\ttechnique\n";
			materialFile << "\t{\n";
				materialFile << "\t\tpass\n";
				materialFile << "\t\t{\n";
					materialFile << "\t\t\ttexture_unit\n";
					materialFile << "\t\t\t{\n";
						materialFile << "\t\t\t\ttexture " << name.ToString() << ".bmp\n";
					materialFile << "\t\t\t}\n";
				materialFile << "\t\t}\n";
			materialFile << "\t}\n";							
		materialFile << "}\n";
	}
}

void OgreExporter_c::CreateResourcesCfg()
{
	fs::path path(strOutputDirectory);
	path /= "Resources.cfg";

	std::ofstream output(path.string().c_str());

	output << "[General]\n";

	output << "FileSystem=" << strMaterialScriptsDirectory << "\n";
	output << "FileSystem=" << strMaterialTexturesDirectory << "\n";
	output << "FileSystem=" << strLevelDirectory << "\n";
}
