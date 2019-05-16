/*================================================================================//
ARCamera
 - DESCRIPTION: Class used to manage a camera using ARToolkit.
 - AUTHOR: Glen K. Straughn
 - DATE: 4/6/2017
 - COMPILER: Visual Studio 2015
//================================================================================*/

#pragma once
#include <string>
#include <AR/video.h>

#include "TypeDef.hpp"
#include "Texture.hpp"

class ARCamera
{
public:
	ARCamera(std::string cameraParameters);
	~ARCamera();

	bool startCamera();
	void stopCamera();

	//Image getCameraFrame();
	void getCameraFrame(Image &imageToUpdate);
	
	// GETTERS AND SETTERS
	inline bool isCameraRunning() { return m_cameraRunning; }
	inline ARParamLT *getCameraParamLTPtr() { return mp_cameraParamLT; }
	inline AR_PIXEL_FORMAT getPixelFormat() { return m_pixelFormat; }

private:
	bool m_cameraRunning;
	AR_PIXEL_FORMAT m_pixelFormat;
	ARParamLT* mp_cameraParamLT;
};


//--------------------------------------------------------------------------------//

// INITIALIZATION ERRORS
enum class CameraInitError
{
	VIDEO_OPEN_ERROR,
	VIDEO_GET_SIZE_ERROR,
	VIDEO_PIXEL_FORMAT_ERROR,
	AR_PARAM_LOAD_ERROR,
	AR_PARAM_LT_CREATE_ERROR
};