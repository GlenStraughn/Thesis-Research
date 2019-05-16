#include "NFTMarker.hpp"

#include <fstream>
#include <KPM/kpm.h>
#include <ar/gsub_lite.h>
#include <glm/ext.hpp>

#include "Parsing.h"
#include "Util.hpp"

NFTMarker::NFTMarker()
{
	static UID nextID = 0;

	m_state = MarkerState::UNREGISTERED;
	m_markerID = nextID;
	m_filterCutoffFrequency = AR_FILTER_TRANS_MAT_CUTOFF_FREQ_DEFAULT;
	m_filterSampleRate = AR_FILTER_TRANS_MAT_SAMPLE_RATE_DEFAULT;
	m_offset = IDENTITY_MATRIX_4X4;

	mp_ftmi = NULL;
	nextID++;
}

//----------------------------------------------------------------------//

NFTMarker::NFTMarker(std::string& markerName, ARfloat cutOffFreqency, ARfloat filterSampleRate)
	: NFTMarker::NFTMarker()
{
	m_name = markerName;
	m_filterCutoffFrequency = cutOffFreqency;
	m_filterSampleRate = filterSampleRate;
}

//----------------------------------------------------------------------//

NFTMarker::~NFTMarker()
{
	if (mp_surfaceSet)
	{
		ar2FreeSurfaceSet(&mp_surfaceSet);
	}
	if (mp_ftmi)
	{
		arFilterTransMatFinal(mp_ftmi);
	}
	if (mp_kpmHandle)
	{
		kpmDeleteHandle(&mp_kpmHandle);
	}
}

//----------------------------------------------------------------------//

bool NFTMarker::init(const std::string &dataSetPath, ARParamLT* p_cameraParam, AR_PIXEL_FORMAT pixelFormat)
{
	mp_kpmHandle = kpmCreateHandle(p_cameraParam, pixelFormat);

	mp_ftmi = arFilterTransMatInit(m_filterSampleRate, m_filterCutoffFrequency);

	KpmRefDataSet* p_refDataSet = NULL;
	if (kpmLoadRefDataSet(dataSetPath.c_str(), "fset3", &p_refDataSet) < 0)
	{
		return false; // Not good, apparently...
	}

	/*if (kpmChangePageNoOfRefDataSet(p_refDataSet, KpmChangePageNoAllPages, p_refDataSet->pageNum + 1) < 0)
	{
		return false;
	}
	if (kpmMergeRefDataSet(&p_cummulativeKPMRefDataSet, &p_refDataSet) < 0)
	{
		return false;
	}*/
	if ((mp_surfaceSet = ar2ReadSurfaceSet(dataSetPath.c_str(), "fset", NULL)) == NULL)
	{
		return false;
	}

	kpmSetRefDataSet(mp_kpmHandle, p_refDataSet);
	m_offset = IDENTITY_MATRIX_4X4;

	return true;
}

//----------------------------------------------------------------------//


bool NFTMarker::filterTransformationMatrix()
{
	int reset;
	if (m_state == MarkerState::DETECTED || m_state == MarkerState::LOST)
	{
		reset = 1;
	}
	else
	{
		reset = 0;
	}

	if (mp_ftmi != NULL)
	{
		if (arFilterTransMat(mp_ftmi, m_trans, reset) < 0)
		{
			return false;
		}

		return true;
	}
	else
	{
		return false;
	}
}


//-----------------------------------------------------------------------//


ARPose NFTMarker::getPose() const
{
	ARdouble rawPose[16];
	
	arglCameraViewRH((const ARdouble(*)[4])m_trans, rawPose, 1.0); // Possibly replace 1.0 w/ constant

	return glm::make_mat4x4(rawPose) * m_offset;
}


//----------------------------------------------------------------------//


ARPose NFTMarker::getRawPose() const
{
	ARdouble rawPose[16];

	arglCameraViewRH((const ARdouble(*)[4])m_trans, rawPose, 1.0); // Possibly replace 1.0 w/ constant

	return glm::make_mat4x4(rawPose);
}


//----------------------------------------------------------------------//


ARPose NFTMarker::getOffset() const
{
	return m_offset;
}


//----------------------------------------------------------------------//


bool NFTMarker::isValid() const
{
	if (m_state == MarkerState::DETECTED || m_state == MarkerState::TRACKING)
	{
		return true;
	}
	else
	{
		return false;
	}
}


//----------------------------------------------------------------------//


