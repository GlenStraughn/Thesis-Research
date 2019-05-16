#include "NFTManager.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <exception>
#include <thread>
#include <AR/param.h>
#include <glm/ext.hpp>

#include "Util.hpp"
#include "StringAndNumberConversion.hpp"
#include "Parsing.h"


NFTManager::NFTManager()
{
	m_errorTolerance =	3.0f;
	m_running		 =	false;
	m_findingMarkers =	false;
	mp_AR2Handle	 =	NULL;
	mp_cameraFrame	 =	NULL;
	mp_kpmHandle	 =	NULL;
}


//--------------------------------------------------------------------------------//


NFTManager::~NFTManager()
{
	for (int i = 0; i < m_markers.size(); i++)
	{
		delete m_markers[i]; // Delete markers
	}
	m_markers.clear(); // Remove pointers from vector

	delete mp_camera;
	delete mp_cameraFrame;

	kpmDeleteHandle(&mp_kpmHandle);
	ar2DeleteHandle(&mp_AR2Handle);
}


//--------------------------------------------------------------------------------//


void NFTManager::updateCameraFrame()
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


void NFTManager::updateMarkers()
{
	if (!m_findingMarkers)
	{
		m_findingMarkers = true;
		memcpy(mp_kpmImage->getPixelBuffer(), mp_cameraFrame->getPixelBuffer(), mp_kpmImage->getSize());

		std::thread findMarkerThread(findMarkers, this);
		findMarkerThread.detach();//*/
	}

	ARfloat transform[3][4];
	AR2SurfaceSetT* curSurfaceSet; // Syntactic sugar
	ubyte* cameraFrame = mp_cameraFrame->getPixelBuffer(); // Syntactic sugar

	int arResult;
	ARfloat error;

	for (int i = 0; i < m_markers.size(); i++)
	{
		if (m_markers[i]->isValid())
		{
			curSurfaceSet = m_markers[i]->getSurfaceSetPtr();
			arResult = ar2Tracking(mp_AR2Handle, curSurfaceSet, cameraFrame, transform, &error);
			if (arResult < 0)
			{
				m_markers[i]->setState(MarkerState::LOST);
				m_markers[i]->setError(-1);
			}
			else
			{
				m_markers[i]->setTransform(transform);
				m_markers[i]->filterTransformationMatrix();
				m_markers[i]->setError(error);
				m_markers[i]->setState(MarkerState::TRACKING);
			}
		}
		else
		{
			m_markers[i]->setState(MarkerState::UNREGISTERED);
		}
		
		if(m_markers[i]->getState() == MarkerState::LOST)
		{
			// Marker has just stopped being visible.
			// Put stuff here?
			std::cout << "Lost marker " << m_markers[i]->getMarkerID() << " location" << std::endl;
		}
	}
}


//--------------------------------------------------------------------------------//


//void NFTManager::
void findMarkers(NFTManager *mgr)
{
	/*
	// INITIALIZATIONS
	KpmResult* keyPointResult = NULL; // Points to an array in mp_kpmHandle (does not need to be freed)
	int numberOfResults;


	// Create easy way to track the most accurate results and apply them
	// to the corresponding markers.
	typedef std::pair<KpmResult*, NFTMarker*> KpmResultAndNFTMarker;
	typedef std::map<int, KpmResultAndNFTMarker> KpmResultAndMarkerPtrMap;
	

	KpmResultAndMarkerPtrMap errors;
	for (int i = 0; i < mgr->m_markers.size(); i++)
	{
		errors[mgr->m_markers[i]->getMarkerID()] = KpmResultAndNFTMarker(NULL, mgr->m_markers[i]);
	}


	// Get results
	kpmGetResult(mgr->mp_kpmHandle, &keyPointResult, &numberOfResults); // keyPointResult points to an array in mp_kpmHandle
	if (keyPointResult == NULL)
	{
		throw(Error::ARNullPointerException(
			std::string("ARNullPointer exception in findMarkers(): keyPointResult == NULL")));
		return;
	}

	kpmMatching(mgr->mp_kpmHandle, mgr->mp_kpmImage->getPixelBuffer());
		


	// Update markers using the results with the highest confidence score
	int pageNumber;
	for (int i = 0; i < numberOfResults; i++)
	{
		//pageNumber = keyPointResult[i].pageNo;
		pageNumber = 0;
		if (keyPointResult[i].camPoseF != 0 || errors[pageNumber].second->isValid())
		{
			continue;
		}

		if (errors.find(pageNumber) != errors.end())
		{
			if (errors[pageNumber].first == NULL || keyPointResult[i].error > errors[pageNumber].first->error)
			{
				errors[pageNumber].first = &keyPointResult[i];
			}
		}
	}


	for (KpmResultAndMarkerPtrMap::iterator i = errors.begin(); i != errors.end(); i++)
	{
		if (i->second.first != NULL)
		{
			i->second.second->setState(MarkerState::DETECTED);
			ar2SetInitTrans(i->second.second->getSurfaceSetPtr(), i->second.first->camPose);
		}
	}

	
	//*/
	
	KpmResult* p_results = NULL, *p_bestResult = NULL;
	int numberOfResults = 0;
	bool goodChoice = false;
	float lowestError = mgr->m_errorTolerance; // Makes sure all valid choices are below error threshold.

	for (int i = 0; i < mgr->m_markers.size(); i++)
	{
		if (mgr->m_markers[i]->isValid())
		{
			continue; // No need to check markers that are already detected.
		}

		kpmMatching(mgr->m_markers[i]->getKpmHandlePtr(), mgr->mp_kpmImage->getPixelBuffer());
		kpmGetResult(mgr->m_markers[i]->getKpmHandlePtr(), &p_results, &numberOfResults);

		if (numberOfResults > 0)
		{
			for (int j = 0; j < numberOfResults; j++)
			{
				if (p_results[i].error < lowestError && p_results[i].error >= 0 && p_results[i].camPoseF == 0)
				{
					p_bestResult = &p_results[i];
					lowestError = p_results[i].error;
					goodChoice = true;
				}
			}

			if (goodChoice)
			{
				ar2SetInitTrans(mgr->m_markers[i]->getSurfaceSetPtr(), p_bestResult->camPose);
				mgr->m_markers[i]->setState(MarkerState::DETECTED);
				std::cout << "Found marker " << mgr->m_markers[i]->getMarkerID() << std::endl;
			}
		}


	}
	//*/
	mgr->m_findingMarkers = false;
}


//--------------------------------------------------------------------------------//


bool NFTManager::initManager()
{
	ARParamLT* p_cameraParam = mp_camera->getCameraParamLTPtr();
	mp_AR2Handle = ar2CreateHandle(p_cameraParam, mp_camera->getPixelFormat(), AR2_TRACKING_DEFAULT_THREAD_NUM);
	mp_kpmHandle = kpmCreateHandle(p_cameraParam, mp_camera->getPixelFormat());

	if (mp_AR2Handle == nullptr)
	{
		return false;
	}

	if (threadGetCPU() <= 1)
	{
		ARLOGi("Using NFT tracking settings for a single CPU.\n");
		ar2SetTrackingThresh(mp_AR2Handle, m_errorTolerance);
		ar2SetSimThresh(mp_AR2Handle, 0.50);
		ar2SetSearchFeatureNum(mp_AR2Handle, 16);
		ar2SetSearchSize(mp_AR2Handle, 6);
		ar2SetTemplateSize1(mp_AR2Handle, 6);
		ar2SetTemplateSize2(mp_AR2Handle, 6);
	}
	else
	{
		ARLOGi("Using NFT tracking settings for more than one CPU.\n");
		ar2SetTrackingThresh(mp_AR2Handle, m_errorTolerance);
		ar2SetSimThresh(mp_AR2Handle, 0.50);
		ar2SetSearchFeatureNum(mp_AR2Handle, 16);
		ar2SetSearchSize(mp_AR2Handle, 12);
		ar2SetTemplateSize1(mp_AR2Handle, 6);
		ar2SetTemplateSize2(mp_AR2Handle, 6);
	}

	Image::ColorDepth colorDepth;
	int height, width;
	colorDepth = (Image::ColorDepth)arUtilGetPixelSize(mp_camera->getPixelFormat());
	height = p_cameraParam->param.ysize;
	width = p_cameraParam->param.xsize;

	mp_cameraFrame = new Image(width, height, colorDepth);
	mp_kpmImage = new Image(width, height, colorDepth);
	if (mp_cameraFrame == nullptr || mp_kpmImage == nullptr)
	{
		return false;
	}

	return true;
}


//--------------------------------------------------------------------------------//


bool NFTManager::loadMarkers(std::string &markerFilePath)
{
	// INITIALIZE INPUT FILE
	std::ifstream inputFile(markerFilePath.c_str());
	
	if (!inputFile.good())
	{
		inputFile.close();
		return false;
	}

	// GET NUMBER OF MARKERS IN SCENE FROM CONFIG FILE
	std::string line, remainder, token, name;
	const string DELIMITERS = " \t";

	line = getNextLine(inputFile, true);
	line = tokenize(line, remainder, DELIMITERS);
	int numMarkers = stringToNumber<int>(line);


	// READ EACH MARKER'S INFORMATION FROM CONFIG FILE
	std::string fileLocation, type;
	bool nameDetected;
	ARfloat filterCutOffFreq;
	NFTMarker* p_marker;
	KpmRefDataSet* p_cumulativeKpmRefData = NULL;
	glm::mat4x4 offset;

	for (int i = 0; i < numMarkers && inputFile.good(); i++)
	{
		filterCutOffFreq = AR_FILTER_TRANS_MAT_CUTOFF_FREQ_DEFAULT;
		offset = IDENTITY_MATRIX_4X4;
		nameDetected = false;

		fileLocation = getNextLine(inputFile, true);
		type = getNextLine(inputFile, true);

		if (type != "NFT") { continue; }

		// PUT DO WHILE LOOP HERE
		do
		{
			line = getNextLine(inputFile, false); // Assume filter setting present for time being.
			line = tokenize(line, remainder, DELIMITERS);
			if (line == "FILTER")
			{
				filterCutOffFreq = stringToNumber<ARfloat>(remainder);
			}
			else if (line == "OFFSET")
			{
				offset = readMatrix(inputFile);
			}
			else if (line == "NAME")
			{
				name = getFirstRegionBetween(remainder, "\"", "\"");
				nameDetected = true;
			}
		} while (!line.empty());


		if (!nameDetected)
		{
			name = fileLocation;
		}

		p_marker = new NFTMarker(name, filterCutOffFreq);
		if (p_marker->init(fileLocation, mp_camera->getCameraParamLTPtr(), mp_camera->getPixelFormat()))
		{
			p_marker->setOffset(offset);
			m_markers.push_back(p_marker);
		}
		else
		{
			delete p_marker;
			continue;
		}

	} // end for

	inputFile.close(); // Done with input file, close here to avoid if-statement hedge-maze

	// DEBUG
	KpmRefDataSet* rdSet, *getPtr;
	KpmHandle* h;
	for (int i = 0; i < m_markers.size(); i++)
	{
		kpmGetRefDataSet( m_markers[i]->getKpmHandlePtr(), &getPtr );
		rdSet = (KpmRefDataSet*)malloc(sizeof(*getPtr));
		memcpy(rdSet, getPtr, sizeof(*getPtr));
		kpmMergeRefDataSet(&p_cumulativeKpmRefData, &rdSet);
	}


	// SET INTERNAL VARIABLES
	if (kpmSetRefDataSet(mp_kpmHandle, p_cumulativeKpmRefData) < 0)
	{
		kpmDeleteRefDataSet(&p_cumulativeKpmRefData);
		return false;
	}
	kpmDeleteRefDataSet(&p_cumulativeKpmRefData); // Is this right?

	return true;
}


//----------------------------------------------------------------------//


bool NFTManager::initCamera(std::string &cameraParameterFilePath)
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
	
	return true;
}


//--------------------------------------------------------------------------------//


ARPose NFTManager::getMarkerPose(int markerID) const
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


ARPose NFTManager::getMarkerPose(const std::string &markerName) const
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


ARPose NFTManager::getRawMarkerPose(int markerID) const
{
	for (int i = 0; i < m_markers.size(); i++)
	{
		if (m_markers[i]->getMarkerID() == markerID)
		{
			if (m_markers[i]->isValid() == false)
				break;
			else
				return m_markers[i]->getRawPose();
		}
	}

	return ZERO_MATRIX_4X4;
}


ARPose NFTManager::getRawMarkerPose(const std::string &markerName) const
{
	for (int i = 0; i < m_markers.size(); i++)
	{
		if (m_markers[i]->getName() == markerName)
		{
			if (m_markers[i]->isValid() == false)
				break;
			else
				return m_markers[i]->getRawPose();
		}
	}

	return ZERO_MATRIX_4X4;
}


//---------------------------------------------------------------------------------//


int NFTManager::getMarkerPageNumber(std::string &markerName) const
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


bool NFTManager::start()
{
	if (mp_camera->startCamera())
	{
		m_running = true;
		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------//


void NFTManager::stop()
{
	mp_camera->stopCamera();
	m_running = false;
}


//--------------------------------------------------------------------------------//

/*
bool NFTManager::isMarkerValid(int pageNumber) const
{

}


bool NFTManager::isMarkerValid(std::string &markerName) const
{

}
*/

//--------------------------------------------------------------------------------//


float NFTManager::getMarkerError(int markerID) const
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


ARPose NFTManager::getMarkerOffset(int markerID) const
{
	for (int i = 0; i < m_markers.size(); i++)
	{
		if (m_markers[i]->getMarkerID() == markerID)
		{
			if (m_markers[i]->isValid())
			{
				return m_markers[i]->getOffset();
			}
			else
			{
				return ZERO_MATRIX_4X4;
			}
		}
	}

	return ZERO_MATRIX_4X4;
}