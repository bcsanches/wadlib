#include "OgreExporter.h"

#include <OgreDefaultHardwareBufferManager.h>
#include <OgreLodStrategyManager.h>
#include <OgreLogManager.h>
#include <OgreManualObject.h>
#include <OgreMeshManager.h>
#include <OgreMeshSerializer.h>
#include <OgreResourceGroupManager.h>

#include <boost/filesystem.hpp>

#include "WadFile.h"
#include "WadLevel.h"
#include "WadTypes.h"

OgreExporter_c::OgreExporter_c(const char *outputDirectory):	
	strOutputDirectory(outputDirectory)
{
	//empty
}

void OgreExporter_c::CreateDirectories()
{
	namespace fs = boost::filesystem;

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
	this->CreateDirectories();

	Ogre::DefaultHardwareBufferManager hardwareBufferManager;
	Ogre::ManualObject manualMesh(level.GetName());

	const char *currentMaterialName = "";

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
		stream << rightSide.uMiddleTexture.ToString() << i;
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
}
