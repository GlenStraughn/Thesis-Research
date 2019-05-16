/*======================================================================//
LightEstimator
~ Implementations for estimating light direction from marker data.
//----------------------------------------------------------------------//
AUTHOR: Glen Straughn
COMPILER: Visual Studio 2013
PROJECT: Master's thesis; Light Estimation with Dodecahedral Markers.
DATE: 9/21/2017
//======================================================================*/

#include "LightEstimator.hpp"

#include <fstream>
#include <algorithm>
#include <glm/ext.hpp>

#include "Parsing.h"
#include "Util.hpp"

//======================================================================//
// CONSTANT INITIALIZATIONS
//======================================================================//

const float LightEstimator::m_MAX_AMBIENT = 0.5f;
const float LightEstimator::m_DEFAULT_AMBIENT = 0.3f;


//======================================================================//
// FUNCTIONS
//======================================================================//

LightEstimator::LightEstimator()
{
	m_shadowThreshold = .5f;
}


//--------------------------------------------------------------------------------//


bool LightEstimator::init(const std::string& adjacencyMatrixDescriptionFile)
{
	// INITIALIZE ADJACENCY MATRIX
	std::ifstream inFile(adjacencyMatrixDescriptionFile.c_str());

	if (!inFile.good())
	{
		return false;
	}

	std::string line, remainder, token;
	for (int i = 0; i < 12; i++)
	{
		if (!inFile.good())
		{
			return false;
		}

		getline(inFile, line);

		for (int j = 0; j < 12; j++)
		{
			token = tokenize(line, remainder, "\t, |");

			if (token == "o")
			{
				m_markers[i].adjacency[j] = true;
			}
			else if (token == "x")
			{
				m_markers[i].adjacency[j] = false;
			}

			line = remainder;
		}
	}

	return true;
}


//--------------------------------------------------------------------------------//


glm::vec3 LightEstimator::getLightDirection()
{
	std::vector<glm::vec3> lightVectors;
	float highestLuminance = getHighestLuminance();
	glm::vec4	curNorm;
	
	float ambient = getAmbient();
	float shadowThreshold = (highestLuminance + ambient) / 2; // If luminance < this, then face is in shadow.
	
	if (m_shadowThreshold >= 0)
	{
		shadowThreshold = m_shadowThreshold;
	}
	

	for (int i = 0; i < m_NUMBER_OF_MARKERS; i++)
	{
		if (m_markers[i].luminance >= 0)
		{
			curNorm = m_markers[i].normalVec;

			if (m_markers[i].luminance >= shadowThreshold)
			{
				glm::vec4 lightDirection = m_markers[i].luminance*glm::normalize(curNorm); // Weight using luminance
				lightVectors.push_back(glm::vec3(lightDirection));
			}
			else
			{
				float relativeDarkness = 
					1.0 - ((m_markers[i].luminance - ambient) / (shadowThreshold - ambient));

				glm::vec4 adjacentNorm, light;
				int count = 0;

				for (int j = 0; j < m_NUMBER_OF_MARKERS; j++)
				{
					if (m_markers[i].adjacency[j] && m_markers[j].luminance > shadowThreshold)
					{
						adjacentNorm = m_markers[j].normalVec;
						light += relativeDarkness*glm::normalize(glm::reflect(-curNorm, adjacentNorm));
						count++;
					}
				}				

				if (count > 0)
				{
					lightVectors.push_back(glm::vec3(light / count));
				}
				else
				{
					lightVectors.push_back(relativeDarkness*glm::normalize(glm::vec3(-curNorm)));
				}
			}

		}
	}

	glm::vec3 sumVector = glm::vec3(0, 0, 0);
	
	for (int i = 0; i < lightVectors.size(); i++)
	{
		sumVector = sumVector + lightVectors[i];
	}


	if (lightVectors.size() > 0)
	{
		return sumVector / lightVectors.size(); // Return average
	}
	else
	{
		return sumVector;
	}
}


//--------------------------------------------------------------------------------//


LightEstimator::MarkerData* LightEstimator::getPointerToHighestLuminanceMarker()
{
	
	MarkerData* p_highest = &m_markers[0];

	for (int i = 1; i < m_NUMBER_OF_MARKERS; i++) // Start at one since p_highest is already set to [0]
	{
		if (m_markers[i].luminance >= 0)
		{
			if (p_highest->luminance > m_markers[i].luminance)
			{
				p_highest = &m_markers[i];
			}
		}
	}

	return p_highest;
	
}


//--------------------------------------------------------------------------------//


LightEstimator::MarkerData* LightEstimator::getPointerToLowestLuminanceMarker()
{
	
	MarkerData* p_lowest = &m_markers[0];
	
	for (int i = 1; i < m_NUMBER_OF_MARKERS; i++) // Start at one since p_highest is already set to [0]
	{
		if (m_markers[i].luminance > 0)
		{
			if (p_lowest->luminance < m_markers[i].luminance)
			{
				p_lowest = &m_markers[i];
			}
		}
	}
	/**/
	return p_lowest;

}


//--------------------------------------------------------------------------------//


float LightEstimator::getAmbient()
{
	float lowest = getPointerToLowestLuminanceMarker()->luminance;


	if (lowest > m_MAX_AMBIENT)
	{
		return m_DEFAULT_AMBIENT;
	}
	else if (lowest <= 0.0)
	{
		return m_DEFAULT_AMBIENT;
	}
	else
	{
		return lowest;
	}
}


//--------------------------------------------------------------------------------//


float LightEstimator::getHighestLuminance()
{
	float max = 0;
	for (int i = 0; i < m_NUMBER_OF_MARKERS; i++)
	{
		if (max < m_markers[i].luminance)
		{
			max = m_markers[i].luminance;
		}
	}

	return max;
}


//--------------------------------------------------------------------------------//


float LightEstimator::getMedianLuminance()
{
	
	std::vector<float> luminances;

	for (const auto& i : m_markers)
	{
		if (i.luminance >= 0) // if valid
		{
			luminances.push_back(i.luminance);
		}
	}
/**/
	std::sort(luminances.begin(), luminances.end());

	int size = luminances.size();
	if (size % 2 == 0)
	{
		return (luminances[size/2] + luminances[(size-1)/2]) / 2;
	}
	else
	{
		return luminances[size / 2];
	}
	
}


//--------------------------------------------------------------------------------//


float LightEstimator::getAverageLuminance()
{
	
	float sum = 0;
	int count = 0;

	for (const auto& i : m_markers)
	{
		if (i.luminance < 0) // if valid
		{
			sum = sum + i.luminance;
			count++;
		}
	}

	return sum / count;
}


//--------------------------------------------------------------------------------//


void LightEstimator::setMarkerLuminance(int pageNo, float luminance)
{
	for (int i = 0; i < m_NUMBER_OF_MARKERS; i++)
	{
		if (m_markers[i].pageNo == pageNo)
		{
			m_markers[i].luminance = luminance;
		}
	}
}


//--------------------------------------------------------------------------------//


void LightEstimator::setMarkerNormal(int markerID, glm::vec4 normal)
{
	for (int i = 0; i < m_NUMBER_OF_MARKERS; i++)
	{
		if (m_markers[i].pageNo == markerID)
		{
			m_markers[i].normalVec = normal;
		}
	}
}