#pragma once

#include "RtMathHelp.h"

#include <string>

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