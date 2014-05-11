#include "OgreExporter.h"

#include <OgreDefaultHardwareBufferManager.h>
#include <OgreIteratorWrapper.h>
#include <OgreLodStrategyManager.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <OgreManualObject.h>
#include <OgreMeshManager.h>
#include <OgreMeshSerializer.h>
#include <OgreResourceGroupManager.h>

#include <iostream>

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

void OgreExporter_c::ExportWallMesh(Ogre::ManualObject &manualMesh, int floorHeight, int ceilingHeight, Name_u textureName, int offsetX, int offsetY, const Vertex_s *vertices, const LineDef_s &lineDef, const WadFile_c &wad)
{
	std::stringstream stream;
	stream << textureName.ToString();
	manualMesh.begin(stream.str(), Ogre::RenderOperation::OT_TRIANGLE_LIST);

	const Texture_s &tex = wad.GetTextureInfo(textureName);

	const Vertex_s &startVertex = vertices[lineDef.iStartVertex];
	const Vertex_s &endVertex = vertices[lineDef.iEndVertex];

	int height = ceilingHeight - floorHeight;	
	int width = Ogre::Vector2(startVertex.iX, startVertex.iY).distance(Ogre::Vector2(endVertex.iX, endVertex.iY));
	
	float offsetU = offsetX / (float) tex.uWidth;
	float offsetV = offsetY / (float) tex.uHeight;	

	float endV = (height / (float)tex.uHeight) + offsetV;
	float endU = (width / (float) tex.uWidth) + offsetU;

	manualMesh.position(-startVertex.iX, floorHeight, startVertex.iY);
	manualMesh.textureCoord(offsetU, endV);

	manualMesh.position(-startVertex.iX, ceilingHeight, startVertex.iY);
	manualMesh.textureCoord(offsetU, offsetV);
	
	manualMesh.position(-endVertex.iX, ceilingHeight, endVertex.iY);
	manualMesh.textureCoord(endU, offsetV);		

	manualMesh.position(-endVertex.iX, floorHeight, endVertex.iY);
	manualMesh.textureCoord(endU, endV);	

	manualMesh.triangle(2, 1, 0);
	manualMesh.triangle(0, 3, 2);

	manualMesh.end();
}


void OgreExporter_c::ExportLevel(const WadLevel_c &level, const WadFile_c &file)
{	
	this->ExportLevelMaterials(level, file);

	Ogre::LogManager logManager;
	logManager.createLog("ogre.log", true, true, false);

	Ogre::ResourceGroupManager resourceGroupManager;
	Ogre::LodStrategyManager logStrategyManager;

	Ogre::MaterialManager materialManager;
	materialManager.initialise();

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
		{
			const SideDef_s &leftSide = sideDefs[lineDefs[i].iLeftSideDef];
			const Sector_s &leftSideSector = sectors[leftSide.iSector];

			const SideDef_s &rightSide = sideDefs[lineDefs[i].iRightSideDef];
			const Sector_s &rightSideSector = sectors[rightSide.iSector];


			if(leftSide.uMiddleTexture.uName != '-')
			{
				this->ExportWallMesh(manualMesh, leftSideSector.iFloorHeight, leftSideSector.iCeilHeight, leftSide.uMiddleTexture, leftSide.iOffsetX, leftSide.iOffsety, vertices, lineDefs[i], file);				
			}

			if(leftSide.uLowerTexture.uName != '-')
			{
				this->ExportWallMesh(manualMesh, leftSideSector.iFloorHeight, leftSideSector.iFloorHeight, leftSide.uLowerTexture, leftSide.iOffsetX, leftSide.iOffsety, vertices, lineDefs[i], file);				
			}

			if(leftSide.uUpperTexture.uName != '-')
			{
				this->ExportWallMesh(manualMesh, rightSideSector.iCeilHeight, leftSideSector.iCeilHeight, leftSide.uUpperTexture, leftSide.iOffsetX, leftSide.iOffsety, vertices, lineDefs[i], file);				
			}
		}
		
		if(lineDefs[i].iRightSideDef >= 0)
		{			
			const SideDef_s &rightSide = sideDefs[lineDefs[i].iRightSideDef];
			const Sector_s &rightSideSector = sectors[rightSide.iSector];

			if(rightSide.uLowerTexture.uName != '-')
			{
				const SideDef_s &leftSide = sideDefs[lineDefs[i].iLeftSideDef];
				const Sector_s &leftSideSector = sectors[leftSide.iSector];

				this->ExportWallMesh(manualMesh, rightSideSector.iFloorHeight, leftSideSector.iFloorHeight, rightSide.uLowerTexture, rightSide.iOffsetX, rightSide.iOffsety, vertices, lineDefs[i], file);				
			}	

			if(rightSide.uMiddleTexture.uName != '-')
			{
				this->ExportWallMesh(manualMesh, rightSideSector.iFloorHeight, rightSideSector.iCeilHeight, rightSide.uMiddleTexture, rightSide.iOffsetX, rightSide.iOffsety, vertices, lineDefs[i], file);				
			}		

			if(rightSide.uUpperTexture.uName != '-')
			{
				const SideDef_s &leftSide = sideDefs[lineDefs[i].iLeftSideDef];
				const Sector_s &leftSideSector = sectors[leftSide.iSector];

				this->ExportWallMesh(manualMesh, leftSideSector.iCeilHeight, rightSideSector.iCeilHeight, rightSide.uUpperTexture, rightSide.iOffsetX, rightSide.iOffsety, vertices, lineDefs[i], file);				
			}	
		}
	}

	namespace fs = boost::filesystem;

	fs::path path(strLevelDirectory);

	std::string levelName = level.GetName();
	levelName += ".mesh";

	path /= levelName;		
	
	Ogre::MeshManager meshManager;	
	Ogre::MeshPtr mesh = manualMesh.convertToMesh(level.GetName());

	Ogre::MeshSerializer serializer;
	serializer.exportMesh(mesh.get(), path.string());

	mesh.setNull();

	resourceGroupManager.shutdownAll();	

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
