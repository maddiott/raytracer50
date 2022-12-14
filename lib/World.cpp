#include "World.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

World::World()
{
}

// This is a basic obj reader
// It only supports vertices and polygon faces
// If you include normals, it won't work
// I convert the assets using blender with everything turned off
void World::LoadObject(const std::string& Filename)
{
    std::uniform_int_distribution<unsigned int> mDistribution(0, 255);

    std::fstream objFile(Filename, std::ios::in);

    std::vector<point3d> vertices;
    std::vector<std::vector<int>> faces;
    std::string::size_type sz;
    std::string::size_type index;

    point3d vertex;
    int face;
    std::vector<int> newFace;

    double r = (uint8_t) mDistribution(mGenerator);
    double g = (uint8_t) mDistribution(mGenerator);
    double b = (uint8_t) mDistribution(mGenerator);

    // Parse the obj file
    if (objFile.is_open())
    {
        std::string objLine;
        while (std::getline(objFile, objLine))
        {
            // Vertex
            // https://cplusplus.com/reference/string/stod/
            if (objLine[0] == 'v')
            {
                // convert string to series of doubles    
                index = 1;
                vertex.x = std::stod(objLine.substr(index), &sz);
                index += sz;
                vertex.y = std::stod(objLine.substr(index), &sz);
                index += sz;
                vertex.z = std::stod(objLine.substr(index));
                vertices.push_back(vertex);
            }
            else if (objLine[0] == 'f')
            {
                newFace.clear();

                index = 1;
                face = std::stoi(objLine.substr(index), &sz);
                index += sz;

                // Obj files are one indexed, c++ is zero
                newFace.push_back(face - 1);
                while (index != objLine.size())
                {
                    face = std::stoi(objLine.substr(index), &sz);
                    index += sz;

                    newFace.push_back(face - 1);
                }

                faces.push_back(newFace);
            }
        }

        // Make the polys
        point3d normalVector(0, 0, 0);
        double normVal = 0;

        for (auto faceTemp : faces)
        {
            std::vector<point3d> polyVert{};
            for (auto faceVert : faceTemp)
            {
                vertex.x = vertices[faceVert].x;
                vertex.y = vertices[faceVert].y;
                vertex.z = vertices[faceVert].z;
                polyVert.push_back(vertex - point3d(1, 1, 1));
            }

            normalVector = crossProduct(polyVert[1] - polyVert[0], polyVert[2] - polyVert[1]);

            normVal = norm3d(normalVector);

            if (normVal > 0)
            {
                normalVector = normalVector / normVal;
            }
            else
            {
                normalVector = point3d(0, 0, 0);
            }

            std::shared_ptr<WorldObject> ObjectTemp = std::make_shared<WorldObject>(WorldObject());
            ObjectTemp->SetShapeType(WorldObjectType::Polygon);

            ObjectTemp->AddPolygon(polygon3d(normalVector, polyVert));

            ObjectTemp->SetColor(colorRgb(r, g, b));

            std::shared_ptr<WorldObject> pObject(ObjectTemp);
            mWorldList.push_back(std::move(pObject));

        }
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

        ObjectTemp->SetColor(colorRgb(r, g, b));
        ObjectTemp->AddSphere(sphereTemp);

        std::unique_ptr<WorldObject> pObject(ObjectTemp);

        mWorldList.push_back(std::move(pObject));
    }
    // end of sphere construction

}

void World::UnloadObject()
{
    mWorldListRot.clear(); 
    mWorldList.clear(); 
}

bool World::TestIntersection(point3d rayOrigin, point3d rayDirection, point3d& normal, colorRgb& color)
{
	point3d intersectionNormal(0, 0, 0);

    // If we have a rotated world render that
    if (mWorldListRot.size() > 0)
    {
    
	    for (auto const &object : mWorldListRot)
	{
		intersectionNormal = object->TestIntersection(rayOrigin, rayDirection);
		
		if (norm3d(intersectionNormal) > 0)
		{
            normal = intersectionNormal;
            color = object->GetColor();
            return true;
		}
	}
    }
    else
    {
        for (auto const& object : mWorldList)
        {
            intersectionNormal = object->TestIntersection(rayOrigin, rayDirection);

            if (norm3d(intersectionNormal) > 0)
            {
                normal = intersectionNormal;
                color = object->GetColor();
                return true;
            }
        }
    }
    

    return false;
}

bool World::ApplyTransformation(point3d translation, double xDeg, double yDeg, double zDeg)
{
    mWorldListRot.clear(); 

    point3d normalVector;
    triangle3d tempTriangle;
    polygon3d tempPoly;
    sphere3d tempSphere;

    // Apply rotations first
    for (auto &object : mWorldList)
    {
        switch (object->GetShapeType())
        {
        case WorldObjectType::Triangle:
        for (auto &triangle : object->mTriangles)
        {

            std::shared_ptr<WorldObject> ObjectTemp = std::make_shared<WorldObject>(WorldObject());
            ObjectTemp->SetShapeType(WorldObjectType::Triangle);

            // Rotate x
            tempTriangle.a = RotateX(triangle.a, xDeg);
            tempTriangle.b = RotateX(triangle.b, xDeg);
            tempTriangle.c = RotateX(triangle.c, xDeg);
            tempTriangle.normal = RotateX(triangle.normal, xDeg);

            // Then y
            tempTriangle.a = RotateY(tempTriangle.a, yDeg);
            tempTriangle.b = RotateY(tempTriangle.b, yDeg);
            tempTriangle.c = RotateY(tempTriangle.c, yDeg);
            tempTriangle.normal = RotateY(tempTriangle.normal, yDeg);

            // Then z
            tempTriangle.a = RotateZ(tempTriangle.a, zDeg);
            tempTriangle.b = RotateZ(tempTriangle.b, zDeg);
            tempTriangle.c = RotateZ(tempTriangle.c, zDeg);
            tempTriangle.normal = RotateZ(tempTriangle.normal, zDeg);

            // The translate
            tempTriangle.a = tempTriangle.a + translation;
            tempTriangle.b = tempTriangle.b + translation;
            tempTriangle.c = tempTriangle.c + translation;

            ObjectTemp->AddTriangle(triangle3d(triangle));

            ObjectTemp->SetColor(object->GetColor());


            std::shared_ptr<WorldObject> pObject(ObjectTemp);
            mWorldListRot.push_back(std::move(pObject));
        }
            break;
        case WorldObjectType::Polygon:
            for (auto& poly : object->mPolygons)
            {
                // Empty the temporary poly
                tempPoly.vertices.clear();

                std::shared_ptr<WorldObject> ObjectTemp = std::make_shared<WorldObject>(WorldObject());
                ObjectTemp->SetShapeType(WorldObjectType::Polygon);

                point3d vertexTemp;
                for (auto& vertex: poly.vertices)
                {
                    vertexTemp = RotateX(vertex, xDeg);
                    vertexTemp = RotateY(vertexTemp, yDeg);
                    vertexTemp = RotateZ(vertexTemp, zDeg);
                    vertexTemp = vertexTemp + translation;
                    tempPoly.vertices.push_back(vertexTemp);
                }

                normalVector = poly.normal;
                normalVector = RotateX(normalVector, xDeg);
                normalVector = RotateY(normalVector, yDeg);
                normalVector = RotateZ(normalVector, zDeg);

                ObjectTemp->AddPolygon(polygon3d(normalVector, tempPoly.vertices));

                ObjectTemp->SetColor(object->GetColor());


                std::shared_ptr<WorldObject> pObject(ObjectTemp);
                mWorldListRot.push_back(std::move(pObject));
            }

            break;

        case WorldObjectType::Sphere:

            break;
        }


    }

    return true;
}

