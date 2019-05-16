//================================================================================//
// AssetLoading.cpp
//	- Functions to load assets.
//--------------------------------------------------------------------------------//
// AUTHOR: Glen Straughn
// DATE: 11.28.2017
// COMPILER: Microsoft Visual C++
//================================================================================//

#pragma once

#include <string>
#include <glm/glm.hpp>
#include <algorithm>

#include "AssetLoading.hpp"
#include "StringAndNumberConversion.hpp"
#include "Parsing.h"

//================================================================================//
// MESHES
//================================================================================//

bool loadOpenGexMesh(const char* filePath, Mesh& destination)
{
	std::ifstream inFile(filePath);

	if (!inFile.good()) { return false; }

	std::string line, remainder, token;

	while (inFile.good())
	{
		getline(inFile, line);
		token = tokenize(line, remainder, " \t");

		if (token == "VertexArray")
		{
			token = getFirstRegionBetween(line, "(", ")");
			
			if (token == "attrib = \"position\"")
			{
				readOpenGexVertexArray(inFile, destination, AttributeType::POSITION);
			}
			else if (token == "attrib = \"normal\"")
			{
				readOpenGexVertexArray(inFile, destination, AttributeType::NORMAL);
			}
			
		}
		else if (token == "IndexArray")
		{
			readOpenGexIndexArray(inFile, destination);
		}
	}

	destination.m_glDrawMode = GL_TRIANGLES;
	destination.initializeBuffers();

	return true; // Successfully read file.
}


//--------------------------------------------------------------------------------//


void readOpenGexVertexArray(std::ifstream &inFile, Mesh& destination, AttributeType type)
{
	int bracesDepth = 0;
	int structureLength;
	std::string line, remainder, token, vertexContents;

	do
	{
		getline(inFile, line);

		bracesDepth += std::count(line.begin(), line.end(), '{');
		bracesDepth -= std::count(line.begin(), line.end(), '}');


		if (type == AttributeType::POSITION)
		{
			glm::vec3 input;
			while (!line.empty())
			{
				vertexContents = getFirstRegionBetween(line, "{", "}", &remainder);
				if (vertexContents.empty()) break;
				line = remainder;
				for (int i = 0; i < 3; i++)
				{
					token = tokenize(vertexContents, remainder, " \t,");
					input[i] = stringToNumber<float>(token);
					vertexContents = remainder;
				}

				destination.m_vertices.push_back(input);
			}

			
		}
		else if (type == AttributeType::NORMAL)
		{
			glm::vec3 input;
			while (!line.empty())
			{
				vertexContents = getFirstRegionBetween(line, "{", "}", &remainder);
				if (vertexContents.empty()) break;
				line = remainder;
				for (int i = 0; i < 3; i++)
				{
					token = tokenize(vertexContents, remainder, " \t,");
					input[i] = stringToNumber<float>(token);
					vertexContents = remainder;
				}

				destination.m_normals.push_back(input);
			}

			
		}
		else if (type == AttributeType::UV_COORDINATES)
		{
			///*
			glm::vec2 input;
			
			while (!line.empty())
			{
				vertexContents = getFirstRegionBetween(line, "{", "}", &remainder);
				if (vertexContents.empty()) break;
				line = remainder;
				for (int i = 0; i < 2; i++)
				{
					token = tokenize(vertexContents, remainder, " \t,");
					input[i] = stringToNumber<float>(token);
				}
				destination.m_uvCoordinates.push_back(input);
			}
			//*/
		}

	} while (bracesDepth > 0);
}


//--------------------------------------------------------------------------------//


void readOpenGexIndexArray(std::ifstream &inFile, Mesh& destination)
{
	int bracesDepth = 0;
	int structureLength;
	std::string line, remainder, token, index;

	do
	{
		getline(inFile, line);

		bracesDepth += std::count(line.begin(), line.end(), '{');
		bracesDepth -= std::count(line.begin(), line.end(), '}');

		while (!line.empty())
		{
			token = getFirstRegionBetween(line, "{", "}", &remainder);
			line = remainder;

			while (!token.empty())
			{
				index = tokenize(token, remainder, " \t,");

				destination.m_indices.push_back(stringToNumber<int>(index));

				token = remainder;
			}
		}
	} while (bracesDepth > 0);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


bool readOBJ(std::string &fileName, Mesh &destination)
{
	// DECLARATIONS
	std::ifstream file(fileName.c_str());
	std::string line, rem, token;
	const std::string DELIMITERS = " \t,";
	std::map<GLuint, GLuint> normalIndeces;

	destination.m_glDrawMode = GL_TRIANGLES;

	if (file.good())
	{
		while (!file.eof())
		{
			std::getline(file, line);
			token = tokenize(line, rem, DELIMITERS);

			if (token == "v") // VERTEX
			{
				glm::vec3 vert(0, 0, 0);
				for (int i = 0; i < 3; i++)
				{
					if (rem.empty())
					{
						break;
					}

					line = rem;
					token = tokenize(line, rem, DELIMITERS);
					vert[i] = stringToNumber<GLfloat>(token);
				}

				destination.m_vertices.push_back(vert);
			}
			else if (token == "vt") // VERTEX TEXTURE COORDINATES
			{
				// Not yet implemented
			}
			else if (token == "vn") // VERTEX NORMAL
			{
				glm::vec3 norm(0, 0, 0);
				for (int i = 0; i < 3; i++)
				{
					if (rem.empty())
					{
						break;
					}

					line = rem;
					token = tokenize(line, rem, DELIMITERS);
					norm[i] = stringToNumber<GLfloat>(token);
				}

				destination.m_normals.push_back(norm);
			}
			else if (token == "f")
			{
				objReadFaceLine(rem, destination, normalIndeces);
			}
		}
	}
	else
	{
		file.close();
		return false;
	}

	// Reorder vertex normal list because vertex indices and normal indices
	// aren't stored in the same order.
	if (destination.m_vertices.size() == destination.m_normals.size())
	{
		for (auto &i : normalIndeces)
		{
			swap(destination.m_normals[i.first], destination.m_normals[i.second]);
		}
	}

	file.close();
	destination.initializeBuffers();
	return true;
}


//--------------------------------------------------------------------------------//


static void objReadFaceLine(std::string &line, Mesh &destination, std::map<GLuint, GLuint> &normIndices)
{
	std::string token, rem, subRem, subToken;
	GLuint index;

	for (int i = 0; i < 3; i++)
	{
		token = tokenize(line, rem, " \t,");

		if (token.find("//") != std::string::npos)
		{
			token = tokenize(token, subRem, "/");
			index = stringToNumber<GLuint>(token)-1;
			destination.m_indices.push_back(index);

			token = subRem;
			token = tokenize(token, subRem, "/");
			normIndices[index] = stringToNumber<GLuint>(token)-1;
		}
		else if (token.find("/") != std::string::npos)
		{
			token = tokenize(token, subRem, "/");
			destination.m_indices.push_back(stringToNumber<GLuint>(token)-1);
			// Add texture stuff
			// Add normal index stuff
		}
		else
		{
			destination.m_indices.push_back(stringToNumber<GLuint>(token)-1);
		}

		if (rem.empty())
		{
			break;
		}

		line = rem;
	}
}


//================================================================================//