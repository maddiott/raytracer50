#pragma once
// This class defines the types of objects that can be present in the scene
// It also implements the intersection routines for each object class

#include "RtMathHelp.h"

#include <cmath>
#include <vector>

enum class WorldObjectType
{
    Point = 0, // lighting
    Sphere = 1,
    Triangle = 2,
    Polygon = 3
};

class WorldObject
{
    public:
        void SetShapeType(WorldObjectType ShapeType);
        WorldObjectType GetShapeType();

        void SetColor(color3 color);
        color3 GetColor();

        void AddTriangle(const triangle3d& NewTriangle);
        void AddPolygon(const polygon3d& NewPolygon);
        void AddSphere(const sphere3d& NewSphere);

        point3d TestIntersection(const point3d& rayOrigin, const point3d& rayDirection);

        
    // Should do a friendship or inheritance, but I'm getting tired
    public:
        // We're handling triangles and polygons as two cases since triangle intersection is easier
        std::vector<triangle3d> mTriangles;
        std::vector<polygon3d> mPolygons;
        std::vector<sphere3d> mSpheres;
        color3 mColor = color3(0, 0, 0);

    private:
        point3d SphereIntersection(const point3d& rayOrigin, const point3d& rayDirection);
        point3d TriangleIntersection(const point3d& rayOrigin, const point3d& rayDirection);
        point3d PolygonIntersection(const point3d& rayOrigin, const point3d& rayDirection);

    private:
        WorldObjectType mShapeType;
};
