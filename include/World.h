#pragma once

#include "RtMathHelp.h"
#include "WorldObject.h"
#include <memory>
#include <random>
#include <string>
#include <vector>

/* I'm just going to use vectors for simplicity
   This project is about raytracing, not complex memory management
*/
class vector;

class World
{
public:
	World();

	void LoadObject(const std::string &Filename);
	void LoadCube();
	void LoadSpheres(int numSpheres);

	bool TestIntersection(point3d rayOrigin, point3d rayDirection, point3d &planeNormal, color3 &color);

	bool ApplyTransformation(point3d translation, double xDeg, double yDeg, double zDeg);

private:
	std::default_random_engine mGenerator;
	std::uniform_int_distribution<unsigned int> mDistribution;
	
	// Need a world list
	std::vector<std::shared_ptr<WorldObject>> mWorldList;
	std::vector<std::shared_ptr<WorldObject>> mWorldListRot;
};