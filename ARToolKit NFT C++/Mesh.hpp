//================================================================================//
// Mesh
//	- Class to represent 3D mesh and file loading functions.
//--------------------------------------------------------------------------------//
// AUTHOR: Glen Straughn
// DATE: 04.06.2017
// COMPILER: Microsoft Visual C++
//================================================================================//

#pragma once

#include<glm\glm.hpp>
#include<GL\glew.h>
#include<vector>
#include<map>

class Mesh
{
public:
	Mesh();
	~Mesh();

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_uvCoordinates;
	std::vector<GLuint> m_indices;

	GLuint m_glDrawMode;

	GLuint m_verticesBufferID;
	GLuint m_normalsBufferID;
	GLuint m_indicesBufferID;

	//=====================================================================//
	// DESCRIPTION: Creates OpenGL buffers and loads them
	// INPUT: None.
	// OUTPUT: None.
	// NOTES: Should be called by loader functions.
	// ====================================================================//
	void initializeBuffers();
};

