/*==================================================================================//
ARCamera.cpp
 - DESCRIPTION: ARCamera class member function implementations.
 - AUTHOR: Glen K.Straughn
 - DATE: 05/23/2017
 - COMPILER: Visual Studio 2015
//==================================================================================*/

#include"ARCamera.hpp"

#include<AR/video.h>
#include<string>

#include "Util.hpp"


ARCamera::ARCamera(std::string cameraParameters)
{
	ARParam cameraParam;

	// OPEN VIDEO
	if (arVideoOpen("") < 0)
	{
		throw(CameraInitError::VIDEO_OPEN_ERROR);
	}

	// GET WIDTH AND HEIGHT
	int imageWidth, imageHeight;
	if (arVideoGetSize(&imageWidth, &imageHeight) < 0)
	{
		throw(CameraInitError::VIDEO_GET_SIZE_ERROR);
		arVideoClose();
	}

	// GET PIXEL FORMAT
	m_pixelFormat = arVideoGetPixelFormat();
	if (m_pixelFormat == AR_PIXEL_FORMAT_INVALID)
	{
		throw(CameraInitError::VIDEO_PIXEL_FORMAT_ERROR);
		arVideoClose();
	}

	// LOAD PARAMETERS FROM FILE
	if (arParamLoad(cameraParameters.c_str(), 1, &cameraParam) < 0)
	{
		throw(CameraInitError::AR_PARAM_LOAD_ERROR);
		arVideoClose();
	}
	// RESIZE IF NECESSARY
	if (cameraParam.xsize != imageWidth || cameraParam.ysize != imageHeight)
	{
		arParamChangeSize(&cameraParam, imageWidth, imageHeight, &cameraParam);
	}

	if ( (mp_cameraParamLT = arParamLTCreate(&cameraParam, AR_PARAM_LT_DEFAULT_OFFSET)) == NULL)
	{
		throw(CameraInitError::AR_PARAM_LT_CREATE_ERROR);
		arVideoClose();
	}
}


//---------------------------------------------------------------------------------//


ARCamera::~ARCamera()
{
	arVideoClose();
	arParamLTFree(&mp_cameraParamLT);
}

//---------------------------------------------------------------------------------//

bool ARCamera::startCamera()
{
	if (arVideoCapStart() != 0)
	{
		return false;
	}
	
	m_cameraRunning = true;
	return true;
}


//---------------------------------------------------------------------------------//


void ARCamera::stopCamera()
{
	m_cameraRunning = false;
	arVideoCapStop();
}


//---------------------------------------------------------------------------------//


void ARCamera::getCameraFrame(Image &imageToUpdate)
{
	
	if (imageToUpdate.getWidth() != mp_cameraParamLT->param.xsize || imageToUpdate.getHeight() != mp_cameraParamLT->param.ysize)
	{
		throw(Error::DimensionsMismatchException());
	}
	else if (imageToUpdate.getColorDepth() != arUtilGetPixelSize(m_pixelFormat))
	{
		throw(Error::DimensionsMismatchException(std::string("Dimension mismatch: color depth")));
	}
	else
	{
		ubyte* p_pixels = imageToUpdate.getPixelBuffer();
		ARUint8* p_capImage = arVideoGetImage();

		if (p_capImage != nullptr)
		{
			memcpy(p_pixels, p_capImage, imageToUpdate.getSize());
		}
		else
		{
			throw(Error::ARNullPointerException(std::string("ARNullPointer Exception in getCameraFrame()")));
		}
	}

}