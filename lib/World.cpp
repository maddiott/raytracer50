#include "World.h"
#include <iostream>

World::World()
{
    //LoadCube();
    LoadSpheres();
}

void World::LoadObject(const string& Filename)
{
    std::uniform_int_distribution<unsigned int> mDistribution(0, 255);
}

void World::LoadCube()
{
    WorldObject* ObjectTemp = new WorldObject();
    ObjectTemp->SetShapeType(WorldObjectType::Polygon);

    /*double vertices[8][3] = {{0.000000, 2.000000, 12.000000},
        {0.000000, 0.000000, 12.000000},
        {2.000000, 0.000000, 12.000000},
        {2.000000, 2.000000, 12.000000},
        {0.000000, 2.000000, 10.000000},
        {0.000000, 0.000000, 10.000000},
        {2.000000, 0.000000, 10.000000},
        {0.000000, 2.000000, 10.000000} };*/

    double vertices[4][3] = {
        {2, 1, 10},
        {1, 2, 10},
        {2, 2, 10},
        {1, 1, 10}
    };

    //int faces[6][4] =
    //    {{1, 2, 3, 4},
    //    {8, 7, 6, 5},
    //    {4, 3, 7, 8},
    //    {5, 1, 4, 8},
    //    {5, 6, 2, 1 },
    //    {2, 6, 7, 3}};
    int faces[1][4] =
    { 1, 2, 3, 4 };

    point3d vertex(0, 0, 0);
    point3d normalVector(0, 0, 0);
    double normVal = 0;

    for (int i = 0; i < 1; i++)
    {
        std::vector<point3d> polyVert{};
        for (int j = 0; j < 4; j++)
        {
            vertex.x = vertices[faces[i][j] - 1][0];
            vertex.y = vertices[faces[i][j] - 1][1];
            vertex.z = vertices[faces[i][j] - 1][2];
            polyVert.push_back(vertex);
        }
        normalVector = crossProduct(polyVert[1] - polyVert[0], polyVert[2] - polyVert[1]);
        std::cout << "x: " << normalVector.x
            << ", y: " << normalVector.y
            << ", z: " << normalVector.z << '\n';

        normVal = norm3d(normalVector);

        if (normVal > 0)
        {
            normalVector = normalVector / normVal;
        }
        else
        {
            normalVector = point3d(0, 0, 0);
        }

        std::cout << "x: " << normalVector.x
            << ", y: " << normalVector.y
            << ", z: " << normalVector.z << '\n';

        std::cout << "norm val: " << normVal << '\n';
        std::cout << "norm of normal vector is: " << norm3d(normalVector) << '\n';
        
        ObjectTemp->AddPolygon(polygon3d(normalVector, polyVert));
    }


    double r = (uint8_t)mDistribution(mGenerator);
    double g = (uint8_t)mDistribution(mGenerator);
    double b = (uint8_t)mDistribution(mGenerator);


    ObjectTemp->SetColor(color3(255, 0, 0));

    std::shared_ptr<WorldObject> pObject(ObjectTemp);

    mWorldList.push_back(std::move(pObject));
}

void World::LoadSpheres()
{
    int NumSpheres = 500;

	// To randomize color and later sampling

    // reset mSpheres
    std::vector<sphere3d> Spheres;

    // this will be a sphere making thing
    // Start with sphere world coord x, y, z
    sphere3d sphereTemp;
    double centX, centY, centZ;

    double radius;

    std::uniform_real_distribution<double> distributionCent(-3, 3);
    std::uniform_real_distribution<double> distributionRadius(0.05, 1);

    for (int i = 0; i < NumSpheres; i++)
    {
        WorldObject* ObjectTemp = new WorldObject();
        ObjectTemp->SetShapeType(WorldObjectType::Sphere);

        centX = ((double)distributionCent(mGenerator));
        centY = ((double)distributionCent(mGenerator));
        centZ = 10;// distributionCent(generator);

        double r = (uint8_t) mDistribution(mGenerator);
        double g = (uint8_t) mDistribution(mGenerator);
        double b = (uint8_t) mDistribution(mGenerator);

        radius = distributionRadius(mGenerator);

        sphereTemp.center = point3d(centX, centY, centZ);
        sphereTemp.radius = 1;// radius;

        ObjectTemp->SetColor(color3(r, g, b));
        ObjectTemp->AddSphere(sphereTemp);

        std::unique_ptr<WorldObject> pObject(ObjectTemp);


        mWorldList.push_back(std::move(pObject));
    }
    // end of sphere construction

}

bool World::TestIntersection(point3d rayOrigin, point3d rayDirection, point3d& normal, color3& color)
{
	point3d intersectionNormal(0, 0, 0);

	for (auto const &object : mWorldList)
	{
		intersectionNormal = object->TestIntersection(rayOrigin, rayDirection);
		
		if (norm3d(intersectionNormal) > 0)
		{
            normal = intersectionNormal;
            color = object->GetColor();
            return true;
		}
	}

    return false;
}
