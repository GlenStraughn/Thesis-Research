/*
//======================================================================//
NFTManager
//----------------------------------------------------------------------//
	DESCRIPTION:
		Manages the initialization, state, and exit routines of the pro-
		gram's AR functionality. Manages each marker and camera.
	AUTHOR: Glen K. Straughn
	DATE: 4/9/2017
	COMPILER: Visual Studio 2015
//======================================================================//
*/
#pragma once

#include <string>
#include <vector>

#include <KPM/kpm.h>
#include <thread_sub.h>
#include <glm/glm.hpp>

#include "NFTMarker.hpp"
#include "ARCamera.hpp"
#include "Texture.hpp"

class NFTManager
{
public:
	// INITILIZATION
	NFTManager();	// Default constructor.
	~NFTManager();	// Destructor

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// DESCRIPTION: Initialize camera and load parameters.
	// MUTATES:
	//	- mp_camera: Instantiates and initializes.
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	bool initCamera(std::string &cameraParameterFilePath);
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// DESCRIPTION: Initialize NFTManager.
	// MUTATES:
	//	- mp_cameraFrame: Instantiates.
	//	- m_AR2Handle: Instantiates.
	// NOTES: Must be called after initCamera
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	bool initManager();
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// DESCRIPTION: Load markers and configurations from file.
	// MUTATES:
	//	- m_markers: Adds markers to list.
	//	- m_kpmHandle: Initializes.
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	bool loadMarkers(std::string &markerFilePath);
	


	// Update subroutines

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// DESCRIPTION: Updates image that stores the camera frame.
	// MUTATES:
	//		- mp_cameraFrame: gets new frame from camera
	// NOTES: Internal use only; called by update()
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	void updateCameraFrame();
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// DESCRIPTION: Performs AR tracking on markers and updates them with results.
	// MUTATES:
	//		- m_markers: If markers appear within frame.
	// NOTES: Internal use only; called by update().
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	void updateMarkers();
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// DESCRIPTION: Starts camera and marker reading.
	// MUTATES:
	//	- m_running
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	bool start();
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// DESCRIPTION: Stops camera and marker reading.
	// MUTATES:
	//	- m_running
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	void stop();
	

	// GETTERS AND SETTERS
	ARPose getMarkerPose(int markerNumber) const;
	ARPose getMarkerPose(const std::string &markerName) const;
	ARPose getRawMarkerPose(int markerNumber) const;
	ARPose getRawMarkerPose(const std::string &markerName) const;
	ARPose getMarkerOffset(int markerNumber) const;
	float getMarkerError(int markerNumber) const;

	inline void setErrorTolerance(float errorTol) { m_errorTolerance = errorTol; }
	inline bool isRunning() { return m_running; }
	inline Image* getCameraFramePtr() const { return mp_cameraFrame; }
	inline ARParamLT* getCameraParamLTPtr() { return mp_camera->getCameraParamLTPtr(); }
	int getMarkerPageNumber(std::string &markerName) const;
	inline AR_PIXEL_FORMAT getARPixelFormat() { return mp_camera->getPixelFormat(); }
	//bool isMarkerValid(int pageNumber) const;
	//bool isMarkerValid(std::string &markerName) const;
	

private:
	float m_errorTolerance;
	bool m_running;

	AR2HandleT			*mp_AR2Handle;
	KpmHandle			*mp_kpmHandle;

	std::vector<NFTMarker*> m_markers;
	ARCamera*				mp_camera;
	Image*					mp_cameraFrame;

	// FIND MARKERS STUFF
	bool m_findingMarkers;
	Image* mp_kpmImage;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// DESCRIPTION: Uses KPM to find markers that are not currently tracked.
	// MUTATES:
	//		- m_markers: If markers appear withing frame.
	// NOTES: Internal use only; called by updateMarkers.
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	friend void findMarkers(NFTManager *mgr);
};