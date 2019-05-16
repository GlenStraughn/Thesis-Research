//=================================================================//
// Shaders.cpp
//-----------------------------------------------------------------//
// Implementations for simple shader class member functions.
//-----------------------------------------------------------------//
// AUTHOR: Glen Straughn
// DATE: 7.19.2016
//=================================================================//

#include "Shaders.hpp"

#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

#include "Util.hpp"


Shader::~Shader()
{
	glDeleteShader(m_handle);
}


//-------------------------------------------------------------------//


bool Shader::compileShaderFromFile(const char * fileName, GLenum type)
{
	std::ifstream file(fileName);

	if (!file.good())
	{
		return false;
	}
	else
	{
		std::ostringstream contents;
		contents << file.rdbuf();	// Looks strange, but quick and easy
		std::string buffer = contents.str();

		return compileShaderFromString(buffer, type);
	}
}


//-------------------------------------------------------------------//


bool Shader::compileShaderFromString(const std::string& source, GLenum type)
{
	m_handle = glCreateShader(type);
	m_type = type;

	const GLchar* cStrSource = source.c_str();
	glShaderSource(m_handle, 1, &cStrSource, nullptr);

	// COMPILATION
	GLint compilationStatus;
	glCompileShader(m_handle);
	glGetShaderiv(m_handle, GL_COMPILE_STATUS, &compilationStatus);
	if (compilationStatus == GL_FALSE)
	{
		GLint infoLogSize;
		glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &infoLogSize);
		GLchar* infoLog = new GLchar[infoLogSize];
		glGetShaderInfoLog(m_handle, infoLogSize, nullptr, infoLog);
		std::cout << infoLog << std::endl;
		delete[] infoLog;

		return false;
	}

	return true;
}


//===================================================================//


ShaderProgram::ShaderProgram()
{
	m_handle = 0;
	m_linked = false;
//	m_logString = "";
}


//-------------------------------------------------------------------//


ShaderProgram::~ShaderProgram()
{
	if (m_handle != 0)
	{
		glDeleteProgram(m_handle);
	}
	glUseProgram(0);
}


//------------------------------------------------------------------//


bool ShaderProgram::link(std::initializer_list<Shader*> argList)
{
	m_handle = glCreateProgram();
	
	// Attach shaders
	for (const Shader* i : argList) // C++11 is awesome, isn't it?
	{
		glAttachShader(m_handle, i->getHandle());
	}

	glLinkProgram(m_handle);

	// Detatch shaders
	for (const Shader* i : argList)
	{
		glDetachShader(m_handle, i->getHandle());
	}

	GLint linkStatus;
	glGetProgramiv(m_handle, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_FALSE)
	{
		GLint linkLogLength;
		glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &linkLogLength);
		GLchar* linkStatusMessage = new GLchar[linkLogLength];
		std::cout << linkStatusMessage << std::endl;
		delete[] linkStatusMessage;

		return false;
	}

	m_linked = true;
	return true;
}


//------------------------------------------------------------------//


GLint ShaderProgram::getUniformLocation(const char* name)
{
	return glGetUniformLocation(m_handle, name);
}


//------------------------------------------------------------------//


void ShaderProgram::setUniform(const char* uniformName, int val)
{
	glUniform1i(getUniformLocation(uniformName), val);
}


//------------------------------------------------------------------//


void ShaderProgram::setUniform(const char* uniformName, float val)
{
	glUniform1f(getUniformLocation(uniformName), val);
}


//------------------------------------------------------------------//


void ShaderProgram::setUniform(const char* uniformName, bool val)
{
	glUniform1i(getUniformLocation(uniformName), val);
}


//------------------------------------------------------------------//


void ShaderProgram::setUniform(const char* uniformName, const glm::vec2& val)
{
	glUniform2fv(getUniformLocation(uniformName), 1, glm::value_ptr(val));
}


//------------------------------------------------------------------//


void ShaderProgram::setUniform(const char* uniformName, const glm::vec3& val)
{
	glUniform3fv(getUniformLocation(uniformName), 1, glm::value_ptr(val));
}


//------------------------------------------------------------------//


void ShaderProgram::setUniform(const char* uniformName, const glm::vec4& val)
{
	glUniform4fv(getUniformLocation(uniformName), 1, glm::value_ptr(val));
}


//------------------------------------------------------------------//


void ShaderProgram::setUniform(const char* uniformName, const glm::mat3x3& val)
{
	glUniformMatrix3fv(getUniformLocation(uniformName), 1, false, glm::value_ptr(val));
}


//------------------------------------------------------------------//


void ShaderProgram::setUniform(const char* uniformName, const glm::mat4x4& val)
{
	GLint loc = getUniformLocation(uniformName);
	if (loc >= 0)
	{
		glUniformMatrix4fv(loc, 1, false, glm::value_ptr(val));
	}
}


//------------------------------------------------------------------//


GLint ShaderProgram::getAttributeLocation(const char* name)
{
	return glGetAttribLocation(m_handle, name);
}


//------------------------------------------------------------------//