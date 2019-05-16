/*======================================================================//
LightEstimator
	~ Class for estimating light direction from marker data.
//----------------------------------------------------------------------//
  AUTHOR: Glen Straughn
  COMPILER: Visual Studio 2013
  PROJECT: Master's thesis; Light Estimation with Dodecahedral Markers.
  DATE: 9/21/2017
//======================================================================*/

#include <vector>
#include <map>
#include <string>
#include <glm/glm.hpp>

#include "Texture.hpp"


class LightEstimator
{
public:
	LightEstimator();
	
	// DESCRIPTION:
	// INPUT: [NONE]
	// OUTPUT: Vector representing the light direction.
	bool init(const std::string& adjacencyMatrixDescriptionFile);

	// DESCRIPTION:
	// INPUT: [NONE]
	// OUTPUT: Vector representing the light direction.
	glm::vec3 getLightDirection();

	// DESCRIPTION: 
	// INPUT: [NONE]
	// OUTPUT: Ambient light intensity [0-1].
	float getAmbient();


	// SETTERS
	void setMarkerLuminance(int pageNo, float luminance);
	inline void setMarkerPageNumber(int index, int pageNo) { m_markers[index].pageNo = pageNo; }
	void setMarkerNormal(int markerID, glm::vec4 normal);
	void setShadowThreshold(float shadowThreshold) { m_shadowThreshold = shadowThreshold; }
	float getHighestLuminance();

protected:
	// CONSTANTS
	static const float m_MAX_AMBIENT;
	static const float m_DEFAULT_AMBIENT;
	static const unsigned int m_NUMBER_OF_MARKERS = 12;

	struct MarkerData
	{
		int pageNo;
		float luminance;
		glm::vec4 normalVec;
		bool adjacency[m_NUMBER_OF_MARKERS];
	};


	MarkerData m_markers[m_NUMBER_OF_MARKERS];

	float m_shadowThreshold;
	

	//============================================================================//
	// PRIVATE FUNCTIONS
	//============================================================================//

	// DESCRIPTION: Finds the markers with the highest luminance.
	// INPUT: [NONE]
	// OUTPUT: Pointer to marker with highest luminance.
	MarkerData* getPointerToHighestLuminanceMarker();

	// DESCRIPTION: Finds the markers with the lowest luminance.
	// INPUT: [NONE]
	// OUTPUT: Pointer to marker with lowest luminance.
	MarkerData* getPointerToLowestLuminanceMarker();

	

	// DESCRIPTION:
	// INPUT: [NONE]
	// OUTPUT: 
	float getMedianLuminance();

	// DESCRIPTION: 
	// INPUT: [NONE]
	// OUTPUT: 
	float getAverageLuminance();
};