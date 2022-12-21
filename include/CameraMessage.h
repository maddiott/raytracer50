#pragma once

#include "RtMathHelp.h"

#include <string>

/* The idea here is that the gui tells the camera what it should do,
   not how it should do it. This message also serves to maintain knowledge
   of the camera (really the renderer) state between the controls and
   implementation.
*/

struct CameraMessage
{
	int mNumRenderThreads;
	double mIlluminationPercentage;
	std::string mObjFilepath;
	double mXAngle;
	double mYAngle;
	double mZAngle;

	point3d mTranslation;
};