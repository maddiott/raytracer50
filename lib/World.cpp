#include "World.h"
#include <iostream>

World::World()
{
}

void World::LoadObject(const string& Filename)
{
    std::uniform_int_distribution<unsigned int> mDistribution(0, 255);
}

void World::LoadCube()
{
    
    constexpr double pi = 3.14159265;

    constexpr double angle = -45;
    double c = cos(angle * pi/180);
    double s = sin(angle * pi/180);

    mat3d rotmatx = { 1, 0, 0,
                      0, c, -s,
                      0, s, c};

    mat3d rotmaty = { c, 0, s,
                      0, 1, 0,
                     -s, 0, c};

    mat3d rotmatz = { c, -s, 0,
                      s, c, 0,
                      0, 0, 1 };

    mat3d eye = { 1, 0, 0,
                  0, 1, 0,
                  0, 0, 1 };

    point3d trans(0, 0, 20);

    point3d vertices[8] = {point3d(-2.000000, 2.000000, 2.000000),
                           point3d(-2.000000, -2.000000, 2.000000),
                           point3d(2.000000, -2.0000000, 2.000000),
                           point3d(2.000000, 2.000000, 2.000000),
                           point3d(-2.000000, 2.000000, -2.000000),
                           point3d(-2.000000, -2.000000, -2.000000),
                           point3d(2.000000, -2.000000, -2.000000),
                           point3d(2.000000, 2.000000, -2.000000) };

    point3d verticesRot[8];

    for (int i = 0; i < 8; i++)
    {
        verticesRot[i] = matMult(rotmatx, matMult(rotmaty, vertices[i])) + matMult(eye, trans);
        vertices[i] = matMult(eye, vertices[i]) + trans;
    }

    /*double vertices[4][3] = {
        {-5, 5, 40},
        {-5, -5, 40},
        {5, -5, 20},
        {5, 5, 20}
    };*/

    int faces[6][4] =
        {{1, 2, 3, 4},
        {8, 7, 6, 5},
        {4, 3, 7, 8},
        {5, 1, 4, 8},
        {5, 6, 2, 1 },
        {2, 6, 7, 3}};
    //int faces[1][4] =
    //{  1, 2, 3, 4};

    point3d vertex(0, 0, 0);
    point3d normalVector(0, 0, 0);
    double normVal = 0;

    for (int i = 0; i < 6; i++)
    {
        std::vector<point3d> polyVert{};
        for (int j = 0; j < 4; j++)
        {
            vertex.x = verticesRot[faces[i][j] - 1].x;
            vertex.y = verticesRot[faces[i][j] - 1].y;
            vertex.z = verticesRot[faces[i][j] - 1].z;
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

        std::shared_ptr<WorldObject> ObjectTemp = std::make_shared<WorldObject>(WorldObject());
        ObjectTemp->SetShapeType(WorldObjectType::Polygon);


        ObjectTemp->AddPolygon(polygon3d(normalVector, polyVert));
        double r = (uint8_t)mDistribution(mGenerator);
        double g = (uint8_t)mDistribution(mGenerator);
        double b = (uint8_t)mDistribution(mGenerator);

//        r = ((i % 2) == 0) ? 255 : 0;
 //       g = ((i % 3) == 1) ? 255 : 0;
  //      b = ((i % 4) == 2) ? 255 : 0;

        ObjectTemp->SetColor(color3(r, g, b));


        std::shared_ptr<WorldObject> pObject(ObjectTemp);
        mWorldList.push_back(std::move(pObject));

    }


}

void World::LoadSpheres(int numSpheres)
{
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

    for (int i = 0; i < numSpheres; i++)
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
        sphereTemp.radius = radius;

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
