//================================================================================//
// Mesh
//	- Implementations for file loading functions.
//--------------------------------------------------------------------------------//
// AUTHOR: Glen Straughn
// DATE: 04.06.2017
// COMPILER: Microsoft Visual C++
//================================================================================//

#include "Mesh.hpp"

#include <fstream>
#include <string>
#include <algorithm>
#include <glm/ext.hpp>

#include "Parsing.h"
#include "StringAndNumberConversion.hpp"

Mesh::Mesh()
{
	m_indicesBufferID = 0;
	m_normalsBufferID = 0;
	m_verticesBufferID = 0;
}


//--------------------------------------------------------------------------------//


Mesh::~Mesh()
{
	int size = m_indices.size()*sizeof(GLuint);
	glDeleteBuffers(size, &m_indicesBufferID);

	size = m_normals.size()*sizeof(glm::vec3);
	glDeleteBuffers(size, &m_normalsBufferID);

	size = m_vertices.size()*sizeof(glm::vec3);
	glDeleteBuffers(size, &m_verticesBufferID);
}


//--------------------------------------------------------------------------------//


void Mesh::initializeBuffers()
{
	int size = m_indices.size() * sizeof(GLuint);
	if (size > 0)
	{
		glGenBuffers(1, &m_indicesBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, &m_indices[0], GL_STATIC_DRAW);
	}

	size = m_normals.size() * sizeof(glm::vec3);
	if (size > 0)
	{
		glGenBuffers(1, &m_normalsBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_normalsBufferID);
		glBufferData(GL_ARRAY_BUFFER, size, glm::value_ptr(m_normals.front()), GL_STATIC_DRAW);
	}

	size = m_vertices.size() * sizeof(glm::vec3);
	if (size > 0)
	{
		glGenBuffers(1, &m_verticesBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_verticesBufferID);
		glBufferData(GL_ARRAY_BUFFER, size, glm::value_ptr(m_vertices.front()), GL_STATIC_DRAW);
	}
}


//================================================================================//


