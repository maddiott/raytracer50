#pragma once

#include "WorldObject.h"
#include <memory>
#include <random>
#include <string>
#include <vector>
using namespace std;

/* I'm just going to use vectors for simplicity
   This project is about raytracing, not complex memory management
*/
class vector;

class World
{
public:
	World();

	void LoadObject(const string &Filename);
	void LoadCube();
	void LoadSpheres(int numSpheres);

	bool TestIntersection(point3d rayOrigin, point3d rayDirection, point3d &planeNormal, color3 &color);

private:
	std::default_random_engine mGenerator;
	std::uniform_int_distribution<unsigned int> mDistribution;
	
	// Need a world list
	std::vector<std::shared_ptr<WorldObject>> mWorldList;
};