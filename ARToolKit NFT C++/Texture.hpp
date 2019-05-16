#pragma once

#include "TypeDef.hpp"

class Texture
{
public:
	static enum ColorDepth
	{
		RGB = 3,
		RGBA
	};

	Texture(unsigned int width, unsigned int height);
	Texture(unsigned int width, unsigned int height, Texture::ColorDepth depth);

	~Texture();

	inline ubyte* getPixelBuffer() { return mp_pixels; }
	inline unsigned int getWidth() { return m_width; }
	inline unsigned int getHeight() { return m_height; }
	inline unsigned int getSize() { return m_height*m_width*m_colorDepth*sizeof(ubyte); }
	inline ColorDepth getColorDepth() { return m_colorDepth; }


private:
	ubyte* mp_pixels;
	unsigned int m_width;
	unsigned int m_height;
	ColorDepth m_colorDepth;
};
typedef Texture Image;

//----------------------------------------------------------------------//