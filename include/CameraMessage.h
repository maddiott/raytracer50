#pragma once
#include "WorldObject.h"

#include <string>

struct CameraMessage
{
	double mIlluminationPercentage;
	std::string mObjFilepath;
	double mXAngle;
	double mYAngle;
	double mZAngle;

	point3d mTranslation;
};