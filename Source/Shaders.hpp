//=================================================================//
// ShaderProgram.hpp
//-----------------------------------------------------------------//
// AUTHOR: Glen K. Straughn
// DATE: 7.18.2016
//-----------------------------------------------------------------//
// Based on shader class tutorial found at:
//	https://www.packtpub.com/books/content/opengl-40-building-c-shader-program-class
//=================================================================//

#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <initializer_list>

class Shader
{
public:
	~Shader();

	inline GLuint getHandle() const { return m_handle; };
	inline GLenum getType() const { return m_type; }

	bool compileShaderFromFile(const char * fileName, GLenum type);
	bool compileShaderFromString(const std::string& source, GLenum type);
protected:
	GLuint m_handle;
	GLenum m_type;
};


//===================================================================//


class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	inline GLint getHandle() const{ return m_handle; }
	inline bool isLinked() const { return m_linked; }
	
	bool link(std::initializer_list<Shader*> argList);
	void use();

	

	//inline std::string log() { return m_logString; }

	// BINDING FUNCTIONS
	//void bindAttributeLocation(GLuint location, const char* name);
	//void setAttributeArray(GLuint bufferLocation, const char* name);

	// UNIFORM SETTERS
	void setUniform(const char* uniformName, int val);
	void setUniform(const char* uniformName, float val);
	void setUniform(const char* uniformName, bool val);
	void setUniform(const char* uniformName, const glm::vec2& val);
	void setUniform(const char* uniformName, const glm::vec3& val);
	void setUniform(const char* uniformName, const glm::vec4& val);
	void setUniform(const char* uniformName, const glm::mat3x3& val);
	void setUniform(const char* uniformName, const glm::mat4x4& val);

	GLint getAttributeLocation(const char* name);

protected:
	GLuint m_handle;
	bool m_linked;
	//std::string m_logString;

	GLint getUniformLocation(const char* name);
};