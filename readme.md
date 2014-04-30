wadlib
======

Tools for converting Doom maps to 3d meshes, the project aim to create a conversion tool to convert doom maps, textures, etc, to modern formats, to allow the rendering in modern APIs.

Supports:

* Levels loading and rendering
* GL Nodes - http://glbsp.sourceforge.net/
* Flats exportation, per level or all flats
* Wall textures exportation, per level or all textures
* Wall exportation to Ogre Mesh

In progress:

* Exports floors and ceilings
* Exports movable parts (doors, elevators, etc) as separated meshes

![Test level with Ogitor](http://wadlib.googlecode.com/files/wadlib_20120422.JPG)
![2d map viewer](http://wadlib.googlecode.com/files/wadlib.png)        

This is not a Doom port, just a tool for creating triangle meshes from doom maps and playing with it on modern engines.
Free for commercial use, open source under the ZLib License.

https://github.com/bcsanches/wadlib

Modules
-------

Right now the project is composed by the following modules:
* wadlid: the main module that contais the library for loading WAD files and processing its parts
* mapviewer: a simple test utility used for rendering 2d views of doom maps
* ogreexportlib: a library for generating Ogre3d (http://www.ogre3d.org/) meshes, material and texture files
* ogreexport: a executable used for exporting Doom data to Ogre

Compiling
---------

For compiling wablib library Visual Studio 2013 (Express) projects are available.

The wadlib project can be compiled with standard C++ libraries and no dependencies are required.

The other projects will require:
* SDL 1.2.15 (http://www.libsdl.org/)
* SDL GFX Primitives (required by mapviewer) (http://cms.ferzkopp.net/index.php/software/13-sdl-gfx)
* Boost (http://www.boost.org/)
* Ogre3D (http://www.ogre3d.org/)

The projects are pre-configured and assume that the following environment variables exists:
* SDL_HOME: points to SDL root directory
* SDL_GFX_HOME: points to SDL GFX root directory
* BOOST_HOME: points to boost home directory
* OGRE_HOME: points to ogre3d home directory

If you have any of those variables available on your system, just setup the environment variables and the program should be able to compile.

Running
----------

After compiling, run mapviewer.exe. It requires the path to a doom wad file and the name of the levels to load, ie:

"f:/games/DOOM 2/doom2.wad" MAP01 MAP02 MAP03 MAP04 MAP05 MAP06 MAP07 MAP08 MAP09 MAP10 MAP11 MAP12 MAP13 MAP14 MAP15 MAP16 MAP17 MAP18 MAP19 MAP20 MAP21 MAP22 MAP23 MAP24 MAP25 MAP26 MAP27 MAP28 MAP29 MAP30 MAP31 MAP32

You should see the first doom level on screen. 

Keys:
* Page Up / Page Down: change levels
* a: export selected level
* n: show line sides
* s: show segments
* p: show partition lines
* t: show things
* u: show subsectors
* v: show vertices