//================================================================================//
// AssetLoading.hpp
//	- Functions to load assets.
//--------------------------------------------------------------------------------//
// AUTHOR: Glen Straughn
// DATE: 11.28.2017
// COMPILER: Microsoft Visual C++
//================================================================================//

#pragma once

#include<glm/glm.hpp>
#include<fstream>

#include "Mesh.hpp"
#include "Texture.hpp"

//================================================================================//
// MESHES
//================================================================================//

enum class AttributeType
{
	POSITION,
	NORMAL,
	UV_COORDINATES
};

//--------------------------------------------------------------------------------//
// DESCRIPTION: Loads the specified OpenGex file into a mesh object.
// OUTPUT: True if load was successful, false if an error was encountered.
// ARGUMENTS:
//	* filePath: Path to the file to read.
//	* destination: Reference to target mesh to put data in.
//--------------------------------------------------------------------------------//
bool loadOpenGexMesh(const char* filePath, Mesh& destination);

void readOpenGexVertexArray(std::ifstream &inFile, Mesh& destination, AttributeType type);
void readOpenGexIndexArray(std::ifstream &inFile, Mesh& destination);


//--------------------------------------------------------------------------------//
// DESCRIPTION: Reads a Wavefront OBJ file and stores it a mesh.
// RETURNS: True if read successful, false if a problem occured.
// ARGUMENTS:
//	- std::string fileName: File path to obj file to be read.
//	- Mesh &destination: Reference to the mesh to store mesh data in.
//--------------------------------------------------------------------------------//
bool readOBJ(std::string &fileName, Mesh &destination);


//--------------------------------------------------------------------------------//
// DESCRIPTION: Reads a face line.
// ARGUMENTS:
//	- std::string line:
//	- Mesh &destination: Reference to the mesh to store mesh data in.
//--------------------------------------------------------------------------------//
static void objReadFaceLine(std::string &line, Mesh &destination, std::map<GLuint, GLuint> &normIndices);



//================================================================================//
// TEXTURES/IMAGES
//================================================================================//

//--------------------------------------------------------------------------------//
// DESCRIPTION: Loads the specified FBX file into a mesh object.
// OUTPUT: True if load was successful, false if an error was encountered.
// ARGUMENTS:
//	* filePath: Path to the file to read.
//	* destination: Reference to target texture to put data in.
//--------------------------------------------------------------------------------//
bool loadPNG(char* filePath, Texture& destination);


