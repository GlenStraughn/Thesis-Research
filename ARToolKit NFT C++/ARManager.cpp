//================================================================================//
// ARManager
//	- Manager class for ARMarkers
//--------------------------------------------------------------------------------//
// AUTHOR: Glen Straughn
// DATE: 01.16.2018
// COMPILER: Microsoft Visual C++
//--------------------------------------------------------------------------------//
// NOTE: This class currently only supports glyph markers, because I have no reason
//		 to incorporate NFT functionality.
//================================================================================//

#include "ARManager.hpp"

#include <iostream>
#include <yaml-cpp/yaml.h>

ARManager::ARManager()
{
	m_running = false;
	m_verbose = false;
	m_errorTolerance = 1.0f;

	mp_camera = nullptr;
	mp_cameraFrame = nullptr;
	mp_arHandle = nullptr;
	mp_ar3dHandle = nullptr;

	m_numberOfPasses = 1;
	m_passIncrement = 20;
	m_baseThreshold = 256 / 2;
}


//--------------------------------------------------------------------------------//


bool ARManager::initCamera(const std::string &cameraParameterFilePath)
{
	try
	{
		if (mp_camera != NULL)
		{
			return false;
		}

		mp_camera = new ARCamera(cameraParameterFilePath);
	}
	catch (CameraInitError &errorCode)
	{
		switch (errorCode)
		{
		case CameraInitError::AR_PARAM_LOAD_ERROR:
			std::cout << "CAMERA INIT: AR_PARAM_LOAD_ERROR." << std::endl;
			break;

		case CameraInitError::AR_PARAM_LT_CREATE_ERROR:
			std::cout << "CAMERA INIT: AR_PARAM_LT_CREATE_ERROR" << std::endl;
			break;

		case CameraInitError::VIDEO_GET_SIZE_ERROR:
			std::cout << "CAMERA INIT: VIDEO GET SIZE ERROR." << std::endl;
			break;

		case CameraInitError::VIDEO_OPEN_ERROR:
			std::cout << "CAMERA INIT: VIDEO_OPEN_ERROR." << std::endl;
			break;

		case CameraInitError::VIDEO_PIXEL_FORMAT_ERROR:
			std::cout << "CAMERA INIT: VIDEO_PIXEL_FORMAT_ERROR." << std::endl;
			break;
		}

		return false;
	}

	if (  (mp_ar3dHandle = ar3DCreateHandle( &(mp_camera->getCameraParamLTPtr()->param) )) == NULL )
	{
		std::cout << "AR 3D Handle creation failed." << std::endl;
		return false;
	}

	return true;
}


//--------------------------------------------------------------------------------//


bool ARManager::initManager()
{
	ARPattHandle* p_patternHandle = arPattCreateHandle();
	ARParamLT* p_cameraParam = mp_camera->getCameraParamLTPtr();
	mp_arHandle = arCreateHandle(p_cameraParam);
	arPattAttach(mp_arHandle, p_patternHandle);

	if (mp_arHandle == nullptr)
	{
		return false;
	}

	Image::ColorDepth colorDepth;
	int height, width;
	colorDepth = (Image::ColorDepth)arUtilGetPixelSize(mp_camera->getPixelFormat());
	height = p_cameraParam->param.ysize;
	width = p_cameraParam->param.xsize;

	mp_cameraFrame = new Image(width, height, colorDepth);
	if (mp_cameraFrame == nullptr)
	{
		return false;
	}

	arGetLabelingThresh(mp_arHandle, &m_baseThreshold);

	return true;
}


//--------------------------------------------------------------------------------//


bool ARManager::loadMarkers(const std::string &markerFilePath)
{
	YAML::Node file;
	std::string path, name;
	MarkerType type;
	ARMarker* p_nextMarker;
	ARPose offset;

	try
	{
		file = YAML::LoadFile(markerFilePath);
	}
	catch (YAML::BadFile &ex)
	{
		return false;
	}
	catch (YAML::ParserException &ex)
	{
		return false;
	}
	catch (YAML::BadSubscript &ex)
	{
		return false;
	}

	// INITIALIZATION LOOP
	for (int i = 0; i < file.size(); i++)
	{
		p_nextMarker = NULL;
		path = name = "";
		offset = IDENTITY_MATRIX_4X4;

		if (file[i]["File Path"])
		{
			path = file[i]["File Path"].as<std::string>();
		}
		if (file[i]["Type"])
		{
			type = parseType(file[i]["Type"].as<std::string>());
		}
		if (file[i]["Name"])
		{
			name = file[i]["Name"].as<std::string>();
		}

		try
		{
			switch (type)
			{
			case MarkerType::GLYPH:
				p_nextMarker = new GlyphMarker(path.c_str(), mp_arHandle, name);
				break;
			case MarkerType::NFT:
				// Not going to worry about this for the time being.
				break;
			default:
				std::cout << "Marker of unknown type at index " << i << "." << std::endl;
			} // End switch block
		}
		catch (Error::ARNullPointerException &ex)
		{
			std::cout << ex.what() << std::endl;
			delete p_nextMarker;
			p_nextMarker = NULL;
		}


		if (file[i]["Offset"])
		{
			for (int r = 0; r < 4; r++)
			{
				for (int c = 0; c < 4; c++)
				{
					offset[c][r] = file[i]["Offset"][r * 4 + c].as<double>();
				}
			}

			p_nextMarker->setOffset(offset);
		} // End offset condition

		if (p_nextMarker != NULL)
		{
			m_markers.push_back(p_nextMarker);
		}
	}

	if (m_markers.empty())
	{
		return false;
	}

	return true;
}


//--------------------------------------------------------------------------------//


void ARManager::updateCameraFrame()
{
	if (mp_cameraFrame != nullptr)
	{
		mp_camera->getCameraFrame(*mp_cameraFrame);
	}
	else
	{
		throw(Error::Exception(std::string("Null pointer exception in updateCameraFrame(): camera not initialized!")));
	}
}


//--------------------------------------------------------------------------------//


void ARManager::updateMarkers()
{
	ARdouble transform[3][4];
	ubyte* p_cameraFrame = mp_cameraFrame->getPixelBuffer(); // Syntactic sugar
	ARfloat error;
	int bestMatch;
	ARInt32	markerNum;
	ARMarkerInfo* p_markerInfo;

	int threshold = m_baseThreshold;

	// Reset all markers' errors to -1
	for (int i = 0; i < m_markers.size(); i++)
	{
		m_markers[i]->setError(-1);
	}

	// MULTIPLE PASSES
	for (int pass = 0; pass < m_numberOfPasses; pass++)
	{
		arSetLabelingThresh(mp_arHandle, threshold);
		arDetectMarker(mp_arHandle, p_cameraFrame);
		markerNum = arGetMarkerNum(mp_arHandle);
		if ((p_markerInfo = arGetMarker(mp_arHandle)) == NULL) continue; // It can be null sometimes.
		
		// MATCH MARKERS TO RESULTS AND PICK BEST ONE
		for (int i = 0; i < m_markers.size(); i++)
		{
			if (m_markers[i]->getType() != MarkerType::GLYPH) continue;
			bestMatch = -1;

			for (int j = 0; j < markerNum; j++)
			{
				if (p_markerInfo[j].id == ((GlyphMarker*)m_markers[i])->getARPatternID() && p_markerInfo[j].cf > m_errorTolerance)
				{
					if (m_markers[i]->getError() < p_markerInfo[j].cf)
					{
						m_markers[i]->setError(p_markerInfo[j].cf);
						bestMatch = j;
					}
				}
			} // END j loop

			if (bestMatch != -1) // Suitible Match found
			{
				arGetTransMatSquare(mp_ar3dHandle, &p_markerInfo[bestMatch], 2.0, transform);
				m_markers[i]->setTransform(makeGLMatrixFromAR(transform));
			}
		}// END i loop

		threshold += m_passIncrement;
	} //END of Pass


	// UPDATE MARKER STATE
	for (int i = 0; i < m_markers.size(); i++)
	{
		if (m_markers[i]->getError() >= 0)
		{ 
			switch (m_markers[i]->getState())
			{
			case MarkerState::LOST:
			case MarkerState::UNREGISTERED:
				m_markers[i]->setState(MarkerState::DETECTED);
				break;
			case MarkerState::DETECTED:
				m_markers[i]->setState(MarkerState::TRACKING);
				break;
			}
		}
		else
		{
			switch (m_markers[i]->getState())
			{
			case MarkerState::DETECTED:
			case MarkerState::TRACKING:
				m_markers[i]->setState(MarkerState::LOST);
				break;
			case MarkerState::LOST:
				m_markers[i]->setState(MarkerState::UNREGISTERED);
			}
		}

		// PRINT OUT STUFF
		if (m_verbose)
		{ 
			if (m_markers[i]->getState() == MarkerState::DETECTED)
			{
				std::cout << "Marker " << m_markers[i]->getMarkerID() << " detected." << endl;
				std::cout << "CONFIDENCE: " << m_markers[i]->getError() << endl;
			}
			else if (m_markers[i]->getState() == MarkerState::LOST)
			{
				std::cout << "Marker " << m_markers[i]->getMarkerID() << " lost." << endl;
			}
		}
	}
}


//--------------------------------------------------------------------------------//


float ARManager::getMarkerError(int markerID) const
{
	for (int i = 0; i < m_markers.size(); i++)
	{
		if (m_markers[i]->getMarkerID() == markerID)
		{
			if (m_markers[i]->isValid())
			{
				return m_markers[i]->getError();
			}
			else
			{
				return -1;
			}
		}
	}

	return -1;
}


//--------------------------------------------------------------------------------//


ARPose ARManager::getMarkerPose(int markerID) const
{
	for (int i = 0; i < m_markers.size(); i++)
	{
		if (m_markers[i]->getMarkerID() == markerID)
		{
			if (m_markers[i]->isValid() == false)
				break;
			else
				return m_markers[i]->getPose();
		}
	}

	return ZERO_MATRIX_4X4;
}


ARPose ARManager::getMarkerPose(const std::string &markerName) const
{
	for (int i = 0; i < m_markers.size(); i++)
	{
		if (m_markers[i]->getName() == markerName)
		{
			if (m_markers[i]->isValid() == false)
				break;
			else
				return m_markers[i]->getPose();
		}
	}

	return ZERO_MATRIX_4X4;
}


//---------------------------------------------------------------------------------//


ARPose ARManager::getOffsetMarkerPose(int markerID) const
{
	for (int i = 0; i < m_markers.size(); i++)
	{
		if (m_markers[i]->getMarkerID() == markerID)
		{
			if (m_markers[i]->isValid() == false)
				break;
			else
				return m_markers[i]->getOffsetPose();
		}
	}

	return ZERO_MATRIX_4X4;
}


ARPose ARManager::getOffsetMarkerPose(const std::string &markerName) const
{
	for (int i = 0; i < m_markers.size(); i++)
	{
		if (m_markers[i]->getName() == markerName)
		{
			if (m_markers[i]->isValid() == false)
				break;
			else
				return m_markers[i]->getOffsetPose();
		}
	}

	return ZERO_MATRIX_4X4;
}


//---------------------------------------------------------------------------------//


ARPose ARManager::getMarkerOffset(int markerNumber) const
{
	for (int i = 0; i < m_markers.size(); i++)
	{
		if (m_markers[i]->getMarkerID() == markerNumber)
		{
			return m_markers[i]->getOffset();
		}
	}

	return ZERO_MATRIX_4X4;
}


ARPose ARManager::getMarkerOffset(const std::string &markerName) const
{
	for (int i = 0; i < m_markers.size(); i++)
	{
		if (m_markers[i]->getName() == markerName)
		{
			return m_markers[i]->getOffset();
		}
	}

	return ZERO_MATRIX_4X4;
}


//---------------------------------------------------------------------------------//


int ARManager::getMarkerPageNumber(std::string &markerName) const
{
	for (int i = 0; i < m_markers.size(); i++)
	{
		if (markerName == m_markers[i]->getName())
		{
			return m_markers[i]->getMarkerID();
		}
	}

	return -1;
}


//--------------------------------------------------------------------------------//


void ARManager::setBaseThreshold(unsigned int threshold)
{
	if (threshold < 0)
	{
		m_baseThreshold = 0;
	}
	else if (threshold > 255)
	{
		m_baseThreshold = 255;
	}
	else
	{
		m_baseThreshold = threshold;
	}
}


//--------------------------------------------------------------------------------//


bool ARManager::start()
{
	if (mp_camera->startCamera())
	{
		m_running = true;
		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------//


void ARManager::stop()
{
	mp_camera->stopCamera();
	m_running = false;
}