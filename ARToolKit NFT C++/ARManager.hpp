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
#pragma once

#include<AR/ar.h>

#include<vector>
#include<string>

#include "ARMarker.hpp"
#include "GlyphMarker.hpp"
#include "ARCamera.hpp"
#include "TypeDef.hpp"

class ARManager
{
public:
	ARManager();

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// DESCRIPTION: Initialize camera and load parameters.
	// MUTATES:
	//	- mp_camera: Instantiates and initializes.
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	bool initCamera(const std::string &cameraParameterFilePath);

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
	bool loadMarkers(const std::string &markerFilePath);



	// Update subroutines

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// DESCRIPTION: Updates image that stores the camera frame.
	// MUTATES:
	//		- mp_cameraFrame: gets new frame from camera
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	void updateCameraFrame();

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	// DESCRIPTION: Performs AR tracking on markers and updates them with results.
	// MUTATES:
	//		- m_markers: If markers appear within frame.
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
	ARPose getOffsetMarkerPose(int markerNumber) const;
	ARPose getOffsetMarkerPose(const std::string &markerName) const;
	ARPose getMarkerOffset(int markerNumber) const;
	ARPose getMarkerOffset(const std::string &markerName) const;
	float getMarkerError(int markerNumber) const;

	inline void setErrorTolerance(float errorTol) { m_errorTolerance = errorTol; }
	inline bool isRunning() { return m_running; }
	inline Image* getCameraFramePtr() const { return mp_cameraFrame; }
	inline ARParamLT* getCameraParamLTPtr() { return mp_camera->getCameraParamLTPtr(); }
	int getMarkerPageNumber(std::string &markerName) const;
	inline AR_PIXEL_FORMAT getARPixelFormat() { return mp_camera->getPixelFormat(); }

	inline ARHandle* getARHandlePtr() { return mp_arHandle; }

	inline void setNumberOfPasses(unsigned int num) { m_numberOfPasses = num; }
	inline void setPassIncrement(int interval) { m_passIncrement = interval; } // Sets the amount to increase threshold between passes.
	void setBaseThreshold(unsigned int threshold);  // Threshold is of range [0, 255]
	int getBaseThreshold() const { return m_baseThreshold; }

	void toggleVerbose() { m_verbose = !m_verbose; }

protected:
	bool m_running;
	bool m_verbose;			// Prints out marker detection when true;
	float m_errorTolerance;	// Lower bound
	unsigned int m_numberOfPasses;	// Number of times to scan mp_cameraFrame
	int m_passIncrement;		// Amount to increment threshold per pass.
	int m_baseThreshold;

	std::vector<ARMarker*> m_markers;
	ARCamera* mp_camera;
	Image* mp_cameraFrame;
	ARHandle* mp_arHandle;
	AR3DHandle* mp_ar3dHandle;
};