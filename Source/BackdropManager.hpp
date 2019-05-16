//===========================================================================//
// BackdropManager.hpp
//	- Library for pasting camera frame to background.
//---------------------------------------------------------------------------//
// AUTHOR: Glen K. Straughn
// DATE: 10/11/2017
// COMPILER: Visual C++
//===========================================================================//

#pragma once

#include<gl/glew.h>

#include "Mesh.hpp"
#include "Texture.hpp"
#include "Shaders.hpp"

class BackdropManager
{
public:
	BackdropManager();

	bool init(const char* vertexShaderFileName = "Shaders/Backdrop.vert", const char* fragmentShaderFileName = "Shaders/Backdrop.frag");

	// DESCRIPTION: Updates the background image. Called at the beginning rendering.
	// OUTPUT: [NONE]
	// INPUT:
	//  - p_cameraFrame: Pointer to image containing the frame from the camera.
	//  - pixelFormat: The pixel format of the camera frame.
	void update(Image* p_cameraFrame, int pixelFormat);


private:
	Mesh m_mesh;
	GLuint m_textureID;
	ShaderProgram m_shaderProgram;
};