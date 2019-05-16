//===========================================================================//
// GLBuffer.hpp
//  - Class to handle OpenGL vertex buffer object.
//---------------------------------------------------------------------------//
// AUTHOR: Glen K. Straughn
// DATE: 10/11/2017
// COMPILER: Visual C++
//===========================================================================//

#include "GLBuffer.hpp"

GLVertexBufferObject::GLVertexBufferObject(GLenum type)
{
	m_type = type;
	glGenBuffers(1, &m_bufferID);
}


//---------------------------------------------------------------------------//


GLVertexBufferObject::~GLVertexBufferObject()
{
	glDeleteBuffers(1, &m_bufferID);
}


//---------------------------------------------------------------------------//


void GLVertexBufferObject::bufferData(unsigned int size, void* p_data, GLenum usageHint)
{
	glBindBuffer(m_type, m_bufferID);
	glBufferData(m_type, size, p_data, usageHint);
	glBindBuffer(m_type, 0);
}