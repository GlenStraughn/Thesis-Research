//================================================================================//
// GlyphMarker
//	- Class to represent an ARToolkit glyph marker.
//--------------------------------------------------------------------------------//
// AUTHOR: Glen Straughn
// DATE: 01.16.2018
// COMPILER: Microsoft Visual C++
//================================================================================//
#include "GlyphMarker.hpp"

GlyphMarker::GlyphMarker(const char* filePath, ARHandle* p_arHandle, const std::string &name)
{
	ARPattHandle* p_patternHandle = p_arHandle->pattHandle;
	m_arPatternID = arPattLoad(p_patternHandle, filePath);//m_arPatternID = arPattLoad(mp_handle, filePath);
	if (m_arPatternID < 0)
	{
		throw Error::Exception("Maximum number of markers already loaded");
	}

	if (name.empty())
	{
		m_name = "Marker_" + numberToString(this->getMarkerID());
	}
	else
	{
		m_name = name;
	}
}


//--------------------------------------------------------------------------------//


GlyphMarker::~GlyphMarker()
{
	
}