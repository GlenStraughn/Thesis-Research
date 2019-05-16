//================================================================================//
// GlyphMarker
//	- Class to represent an ARToolkit glyph marker.
//--------------------------------------------------------------------------------//
// AUTHOR: Glen Straughn
// DATE: 01.16.2018
// COMPILER: Microsoft Visual C++
//================================================================================//
#pragma once

// Parent Class
#include "ARMarker.hpp"

#include<AR/ar.h>
#include<string>

#include "Util.hpp"

class GlyphMarker : public ARMarker
{
public:
	GlyphMarker(const char* filePath, ARHandle* p_arHandle, const std::string &name = "");
	~GlyphMarker();

	inline MarkerType getType() { return MarkerType::GLYPH; }

	inline int getARPatternID() { return m_arPatternID; }

protected:
	int				m_arPatternID;
};