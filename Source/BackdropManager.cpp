//===========================================================================//
// BackdropManager.cpp
//	- Library for pasting camera frame to background.
//---------------------------------------------------------------------------//
// AUTHOR: Glen K. Straughn
// DATE: 10/11/2017
// COMPILER: Visual C++
//===========================================================================//

#include "BackdropManager.hpp"

#include<iostream>

#include <AR/ar.h>

BackdropManager::BackdropManager()
{
	// MESH
	m_mesh.m_indices = { 0, 2, 1, 3 };
	//m_mesh.m_vertices = { glm::vec3(-1.0, -1.0, -1.0), glm::vec3(1.0, -1.0, -1.0), glm::vec3(-1.0, 1.0, -1.0), glm::vec3(1.0, 1.0, -1.0) }; // Right-handed
	m_mesh.m_vertices = { glm::vec3(1.0, -1.0, -1.0), glm::vec3(-1.0, -1.0, -1.0), glm::vec3(1.0, 1.0, -1.0), glm::vec3(-1.0, 1.0, -1.0) };
	m_mesh.m_glDrawMode = GL_TRIANGLE_STRIP;
}


//---------------------------------------------------------------------------//

bool BackdropManager::init(const char* vertexShaderFileName, const char* fragmentShaderFileName)
{
	Shader vertShader, fragShader;
	
	// SHADER
	if (!vertShader.compileShaderFromFile(vertexShaderFileName, GL_VERTEX_SHADER))
	{
		return false;
	}
	if (!fragShader.compileShaderFromFile(fragmentShaderFileName, GL_FRAGMENT_SHADER))
	{
		return false;
	}
	if (!m_shaderProgram.link({ &vertShader, &fragShader }))
	{
		return false;
	}

	// GL TEXTURE BUFFER
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_BORDER);

	m_mesh.initializeBuffers();

	return true;
}

//---------------------------------------------------------------------------//


void BackdropManager::update(Image* p_cameraFrame, int pixelFormat)
{
	GLenum glPixFormat;
	GLenum internalFormat = GL_RGB8;
	
	switch (pixelFormat)
	{
	case AR_PIXEL_FORMAT_RGB:
		glPixFormat = GL_RGB;
		internalFormat = GL_RGB8;
		break;
	case AR_PIXEL_FORMAT_RGBA:
		glPixFormat = GL_RGBA;
		internalFormat = GL_RGBA8;
		break;
	case AR_PIXEL_FORMAT_BGR:
		glPixFormat = GL_BGR;
		internalFormat = GL_RGB8;
		break;
	case AR_PIXEL_FORMAT_BGRA:
		glPixFormat = GL_BGRA;
		internalFormat = GL_RGBA8;
		break;
	default:
		std::cout << "BACKDROP_MANAGER: Pixel format not identified." << std::endl;
		break;
	};

	glUseProgram(m_shaderProgram.getHandle());
	glDisable(GL_DEPTH_TEST);

	// TEXTURE STUFF
	GLint height = p_cameraFrame->getHeight(), width = p_cameraFrame->getWidth();
	
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, glPixFormat, GL_UNSIGNED_BYTE, p_cameraFrame->getPixelBuffer());
	glActiveTexture(GL_TEXTURE0);
	
	glUniform1i(m_textureID, 0);

	// BINDING BUFFERS
	GLint address = m_shaderProgram.getAttributeLocation("a_position");
	glBindBuffer(GL_ARRAY_BUFFER, m_mesh.m_verticesBufferID);
	glVertexAttribPointer(address, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(address);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mesh.m_indicesBufferID);
	glDrawElements(m_mesh.m_glDrawMode, m_mesh.m_indices.size(), GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(address);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glEnable(GL_DEPTH_TEST);
}