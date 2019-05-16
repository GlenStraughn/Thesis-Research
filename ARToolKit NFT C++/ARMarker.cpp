//================================================================================//
// ARMarker.hpp
//	- Abstract class to represent an ARToolkit Marker.
//--------------------------------------------------------------------------------//
// AUTHOR: Glen Straughn
// DATE: 01.16.2018
// COMPILER: Microsoft Visual C++
//================================================================================//
#include "ARMarker.hpp"
#include <AR/gsub_lite.h>

#include "Parsing.h"
#include "StringAndNumberConversion.hpp"


ARMarker::ARMarker()
{
	// INITIALIZE ID
	static UID id = 0;
	m_markerID = id;
	id++;

	//INITIALIZE STATE
	m_state = MarkerState::UNREGISTERED;
}


//================================================================================//


MarkerType parseType(const std::string& text)
{
	std::string workingString = toLower(text);
	
	if (workingString == "glyph") return MarkerType::GLYPH;
	else if (workingString == "nft") return MarkerType::NFT;

	return MarkerType::INVALID; // If no correct marker type is detected
}