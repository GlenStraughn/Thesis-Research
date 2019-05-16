//===========================================================================//
// GLBuffer.hpp
//  - Class to handle OpenGL vertex buffer object.
//---------------------------------------------------------------------------//
// AUTHOR: Glen K. Straughn
// DATE: 10/11/2017
// COMPILER: Visual C++
//===========================================================================//

#pragma once

#include<GL/glew.h>


class GLVertexBufferObject
{
	GLVertexBufferObject(GLenum type);
	~GLVertexBufferObject();

	void bufferData(unsigned int size, void* p_data, GLenum usageHint = GL_STATIC_DRAW);

	bool isValid(){ return (m_bufferID != 0); };
	GLuint getBufferID(){ return m_bufferID; };
	GLuint getType() { return m_type; }

private:
	GLuint m_bufferID;
	GLenum m_type;
};

typedef GLVertexBufferObject VBO;
typedef GLVertexBufferObject GLBuffer;