/*======================================================================//
Sampler
~ Class for gathering luminance samples from marker faces.
//----------------------------------------------------------------------//
AUTHOR: Glen Straughn
COMPILER: Visual Studio 2013
PROJECT: Master's thesis; Light Estimation with Dodecahedral Markers.
DATE: 9/21/2017
//======================================================================*/

#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <AR\ar.h>

#include "Texture.hpp"
#include "Util.hpp"
#include "Parsing.h"

typedef std::pair < glm::vec2, float > SamplePoint; // 2D coordinate and luminance value

class LuminanceSampler
{
public:

	LuminanceSampler(int id, float radius = 1) { m_markerID = id; m_radius = radius; }

	// DESCRIPTION: Checks marker's sample points and calculates luminance.
	// OUTPUT: Average of luminance found at each sample point.
	// INPUT:
	//	* markerPose: Transformation matrix of the marker being sampled.
	//	* frame: Captured frame from the active ARCamera.
	//	* pixelFormat: Pixel format of AR Camera.
	float getAverageLuminance(ARPose markerPose, Image& frame, AR_PIXEL_FORMAT pixelFormat);

	// DESCRIPTION: Reads a sample point description file.
	// OUTPUT: [NONE]
	// INPUT:
	//	* fileName: String containing path and name for file containing sample point metadata.
	void readSamplePointFile(std::string fileName);

	// GETTERS AND SETTERS
	inline int getMarkerID() const { return m_markerID; }
	inline std::vector<SamplePoint> getSamplePoints() const { return m_samplePoints; } // DEBUGGING
	inline ARPose getFaceOffset() { return m_faceOffset; }
	inline void setFaceOffset(ARPose offset) { m_faceOffset = offset; }
	//inline void addSamplePoint(glm::vec2 coordinates, float maxLuminance) { m_samplePoints.push_back(SamplePoint(coordinates, maxLuminance)); }

protected:
	int m_markerID;
	float m_radius;
	ARPose m_faceOffset;
	std::vector<SamplePoint> m_samplePoints;
};


//----------------------------------------------------------------------//


float LuminanceSampler::getAverageLuminance(ARPose markerPose, Image& frame, AR_PIXEL_FORMAT pixelFormat)
{
	float sum = 0;
	int count = 0;
	glm::vec4 position;
	glm::ivec2 pixelCoord;
	glm::vec3 color;
	
	
	for (int i = 0; i < m_samplePoints.size(); i++)
	{
		position = markerPose * glm::vec4(AR_FACE_SCALE_FACTOR * m_samplePoints[i].first, 0, 1);
		pixelCoord = cameraToScreenCoord( glm::vec3(position.x, position.y, position.z),  frame.getWidth(),  frame.getHeight() );

		if (pixelCoord.x < frame.getWidth() && pixelCoord.y < frame.getHeight() && pixelCoord.x >= 0 && pixelCoord.y >= 0)
		{
			color = getRGB(frame, pixelCoord.x, pixelCoord.y);

			if ( pixelFormat == AR_PIXEL_FORMAT_BGR || pixelFormat == AR_PIXEL_FORMAT_BGRA )
			{
				swap(color.r, color.b);
			}
		
			sum += glm::luminosity(color) / m_samplePoints[i].second;
			count++;
		}
	}

	return sum / count;
}


//----------------------------------------------------------------------//


void LuminanceSampler::readSamplePointFile(std::string fileName)
{
	ifstream inFile(fileName);
	std::string token, line, remainder;
	const std::string DELIMITERS = "\t ,";
	
	SamplePoint nextPoint;

	while (inFile.good())
	{
		getline(inFile, line);

		token = tokenize(line, remainder, DELIMITERS);	// X coord
		nextPoint.first.x = stringToNumber<float>(token);
		
		line = remainder;
		token = tokenize(line, remainder, DELIMITERS);	// Y coord
		nextPoint.first.y = stringToNumber<float>(token);

		line = remainder;
		token = tokenize(line, remainder, DELIMITERS);	// Luminance
		nextPoint.second = stringToNumber<float>(token);

		m_samplePoints.push_back(nextPoint);
	}
}