

#include "Texture.hpp"

Texture::Texture(unsigned int width, unsigned int height)
{
	m_width = width;
	m_height = height;
	m_colorDepth = RGBA;

	mp_pixels = new ubyte[m_width*m_height*RGBA]; // Default to RGBA
}

//----------------------------------------------------------------------//

Texture::Texture(unsigned int width, unsigned int height, Texture::ColorDepth depth)
{
	m_width = width;
	m_height = height;
	m_colorDepth = depth;

	mp_pixels = new ubyte[m_width*m_height*depth]; // Use specified color depth
}

//----------------------------------------------------------------------//

Texture::~Texture()
{
	delete[] mp_pixels;
}

//----------------------------------------------------------------------//
