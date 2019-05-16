// Util.cpp
// Miscelaneous functions, structures, and classes

#pragma once

#include <stdio.h>
#include <string>
#include <fstream>
#include <exception>
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>


#include "TypeDef.hpp"
#include "Parsing.h"
#include "StringAndNumberConversion.hpp"
#include "Texture.hpp"


//================================================================================//
// Utility Classes
//================================================================================//
namespace Error
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// Exception:
	//	- DESCRIPTION: Template for custom exception classes.
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	class Exception : public std::exception
	{
	public:
		Exception() { m_whatMsg = "Exception"; }
		Exception(const std::string &exceptionMessage) { m_whatMsg = exceptionMessage; }
		virtual const char* what() { return m_whatMsg.c_str(); };
	protected:
		std::string m_whatMsg;
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// DimensionsMismatchException:
	//	- DESCRIPTION: Exception thrown when the dimensions of two classes are
	//                 incompatible (e.g. attempting to copy one image to another
	//                 with different widths or heights).
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	class DimensionsMismatchException : public Exception
	{
	public:
		DimensionsMismatchException() { m_whatMsg = "Dimension mismatched exception"; };
		DimensionsMismatchException(std::string &exceptionMsg) : Exception(exceptionMsg) { }
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// ARNullPointerException:
	//	- DESCRIPTION: Exception thrown when an ARToolkit function returns a null
	//	  pointer.
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	class ARNullPointerException : public Exception
	{
	public:
		ARNullPointerException() { m_whatMsg = "Exception: ARToolkit function returned null pointer."; }
		ARNullPointerException(std::string &exceptionMsg) : Exception(exceptionMsg) { }
	};
}

//================================================================================//
// MSVS2015 BACKWARDS COMPATIBILITY
//  Stuff needed because Microsoft likes to screw-over developers
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/*
FILE _iob[] = {*stdin, *stdout, *stderr};

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}
*/
//----------------------------------------------------------------------//


static std::string getNextLine(std::ifstream &inputFile, bool skipBlankLines)
{
	std::string line;

	do
	{
		if (!inputFile.good())
		{
			return "";
		}
		getline(inputFile, line);

	} while ((skipBlankLines && line.empty()) || (!line.empty() && line[0] == '#'));

	return line;
}


//--------------------------------------------------------------------------------//


static glm::mat4x4 readMatrix(std::ifstream &inputFile)
{
	glm::mat4x4 result;
	std::string line, token, remainder;

	for (int i = 0; i < 4; i++)
	{
		if (inputFile.good())
		{
			line = getNextLine(inputFile, true);
		}
		else
		{
			return ZERO_MATRIX_4X4;
		}

		for (int j = 0; j < 4; j++)
		{
			token = tokenize(line, remainder, " \t,");
			line = remainder;

			if (!isNumber(token))
			{
				return ZERO_MATRIX_4X4;
			}

			result[j][i] = stringToNumber<float>(token); // Column major assignment.
		}
	}

	return result;
}


//================================================================================//


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// DESCRIPTION: Gets an RGB value from the provided image. Upper left-hand corner
//				is (0,0).
// INPUT:
//	* image: Reference to the image to retrieve color from.
//	* pixelX: X rasterized coordinate.
//	* pixelY: Y rasterized coordinate.
// OUTPUT: RGB value where each value is from 0 to 1.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static glm::vec3 getRGB(Texture& image, unsigned int pixelX, unsigned int pixelY)
{
	ubyte* p_pixels = image.getPixelBuffer();
	glm::vec3 rgb;
	
	int index = (pixelX + pixelY*image.getWidth())*image.getColorDepth();

	if (index >= 0 && index < image.getSize()) // / image.getColorDepth()) // Sanity checking
	{
		rgb.r = (float)p_pixels[index] / 255;
		rgb.g = (float)p_pixels[index + 1] / 255;
		rgb.b = (float)p_pixels[index + 2] / 255;
	}

	return rgb;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// DESCRIPTION: Converts camera space coordinates to screen pixel indeces.
// INPUT:
//	* coordinates: 3D coordinate to be converted.
//	* width: Width of the window in pixels.
//	* height: Height of the window in pixels.
// OUTPUT: vec2 containing the x and y pixel coordinates of the point.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static glm::ivec2 cameraToScreenCoord(const glm::vec3& coordinates, int width, int height)
{
	glm::ivec2 answer;
	glm::vec2  imagePlaneCoord, ndcCoord;

	imagePlaneCoord.x = coordinates.x / coordinates.z;
	imagePlaneCoord.y = coordinates.y / coordinates.z;

	ndcCoord.x = (imagePlaneCoord.x + 1) / 2;
	ndcCoord.y = (imagePlaneCoord.y + 1) / 2;

	answer.x = ndcCoord.x*width;
	answer.y = (1.0 - ndcCoord.y)*height;

	return answer;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// DESCRIPTION: Decomposes a transformation matrix into position, rotation, and
//				scale.
// INPUT:
//	* mat: Transform to decompose.
//	* position: Vector to put position information into.
//	* rotation: Quaternion to put rotation information into.
//	* scale: Vector to put scale information into.
// OUTPUT: [NONE]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static void decomposeMatrix(const ARPose &mat, glm::vec3 &position, glm::quat &rotation, glm::vec3 &scale)
{
	glm::mat4x4 rotationMatrix(0);
	rotationMatrix[3][3] = 1;
	
	position = glm::vec3(mat[3]);
	
	scale[0] = glm::length(mat[0]);
	scale[1] = glm::length(mat[1]);
	scale[2] = glm::length(mat[2]);
	
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			rotationMatrix[i][j] = mat[i][j]/scale[i];
		}
	}

	rotation = glm::quat(rotationMatrix);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// DESCRIPTION: Makes ARToolkit transform into right-handed mat4x4.
// INPUT:
//	* arMatrix: A 3x4 double array obtained from one of ARToolkit's marker transfrom
//	  functions.
// OUTPUT: Mat4x4 form of arMatrix.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static ARPose makeGLMatrixFromAR(ARdouble arMatrix[3][4])
{
	ARdouble rawMatrix[16];
	arglCameraViewRH(arMatrix, rawMatrix, 1.0);
	return glm::make_mat4(rawMatrix);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// DESCRIPTION: Produces a Y-up version of the provided Z-up matrix.
// INPUT:
//	* mat: Matrix to convert. 
// OUTPUT: Y-up equivalent to mat.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static ARPose zToYUp(const ARPose &mat)
{
	ARPose result = mat;
	std::swap(result[1], result[2]);
	result[2] = -result[2];
	return result;
}

//================================================================================//
// DEBUG
//================================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// DESCRIPTION: Sets the RGB value of the specified pixel.
// INPUT:
//	* image: Reference to the image to set pixel.
//	* pixelX: X rasterized coordinate.
//	* pixelY: Y rasterized coordinate.
//	* color: Color to place.
// OUTPUT: [NONE]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static void setPixel(Texture& image, unsigned int pixelX, unsigned int pixelY, Color color)
{
	ubyte* p_pixels = image.getPixelBuffer();

	int index = (pixelX + pixelY*image.getWidth())*image.getColorDepth();

	if (index >= 0 && index < image.getSize()) // / image.getColorDepth()) // Sanity checking
	{
		for (int i = 0; i < 3; i++)
		{
			p_pixels[index + i] = color[i] * 255;
		}
	}
}


static void setX(Texture& image, unsigned int pixelX, unsigned int pixelY, Color color)
{
	
	setPixel(image, pixelX, pixelY, color);
	setPixel(image, pixelX - 1, pixelY - 1, color);
	setPixel(image, pixelX + 1, pixelY - 1, color);
	setPixel(image, pixelX - 1, pixelY + 1, color);
	setPixel(image, pixelX + 1, pixelY + 1, color);
}