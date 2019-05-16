//================================================================================//
// ARMarker.hpp
//	- Abstract class to represent an ARToolkit Marker.
//--------------------------------------------------------------------------------//
// AUTHOR: Glen Straughn
// DATE: 01.16.2018
// COMPILER: Microsoft Visual C++
//================================================================================//
#pragma once

#include<AR/ar.h>
#include<string>

#include "TypeDef.hpp"


// ENUMERATIONS
enum class MarkerState
{
	DETECTED,
	TRACKING,
	LOST,
	UNREGISTERED
};

enum class MarkerType
{
	GLYPH,
	NFT,
	INVALID = -1 // Used as a default value for the function parseType
};



class ARMarker
{
public:
	ARMarker();

	ARPose getPose() const { return m_pose; }
	ARPose getOffsetPose() const { return m_pose * m_offset; }
	inline ARPose getOffset() const { return m_offset; }

	inline bool isValid() const { return (m_state == MarkerState::DETECTED || m_state == MarkerState::TRACKING); }
	inline void setState(MarkerState state) { m_state = state; }
	inline MarkerState getState() const { return m_state; }

	inline void setTransform(ARPose t) { m_pose = t; };

	inline int getMarkerID() const { return m_markerID; }
	inline std::string getName() { return m_name; }

	inline void setOffset(glm::mat4x4 offset) { m_offset = offset; }

	inline ARfloat getError() { return m_error; }
	inline void setError(ARfloat error) { m_error = error; }

	virtual inline MarkerType getType() = 0; // Virtual

protected:
	ARfloat		m_error;
	std::string m_name;
	
	MarkerState m_state;
	ARPose		m_pose;
	ARPose		m_offset;		// Offset for 3D marker tracking
	ARdouble    m_markerWidth;
	ARdouble    m_markerHeight;

private:
	int         m_markerID;
};


//===========================================================================//


//---------------------------------------------------------------------------//
// DESCRIPTION: Utility function to help parse marker types from file contents.
// OUTPUT: String that possibly contains a marker type
// ARGUMENTS:
//	- text: String to be interpreted as a marker type (e.g. "nft").
//---------------------------------------------------------------------------//
MarkerType parseType(const std::string& text);