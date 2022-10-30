#pragma once

#include <cmath>
#include <vector>

struct mat3d
{
    double x00, x01, x02;
    double x10, x11, x12;
    double x20, x21, x22;
};
 
struct point3d
{
    double x, y, z;
};

inline point3d operator+(const point3d& a, const point3d& b)
{
    point3d result(0, 0, 0);

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

inline point3d operator-(const point3d& a, const point3d& b)
{
    point3d result(0, 0, 0);

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;

    return result;
}

inline point3d operator-(const point3d& a)
{
    point3d result(0, 0, 0);

    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;

    return result;
}

inline point3d operator/(const point3d& a, double b)
{
    point3d result(0, 0, 0);

    result.x = a.x / b;
    result.y = a.y / b;
    result.z = a.z / b;
    
    return result;
}

inline point3d operator*(double b, const point3d& a)
{
    point3d result(0, 0, 0);

    result.x = b * a.x;
    result.y = b * a.y;
    result.z = b * a.z;

    return result;
}

inline point3d matMult(const mat3d& a, const point3d& v)
{
    point3d result(0, 0, 0);

    result.x = a.x00 * v.x + a.x01 * v.y + a.x02 * v.z;
    result.y = a.x10 * v.x + a.x11 * v.y + a.x12 * v.z;
    result.z = a.x20 * v.x + a.x21 * v.y + a.x22 * v.z;

    return result;
};

constexpr double pi = 3.14159265;

inline point3d RotateX(point3d v, double angle)
{
    double c = cos(angle * pi / 180);
    double s = sin(angle * pi / 180);

    mat3d rotmatx = { 1, 0, 0,
                      0, c, -s,
                      0, s, c };

    return matMult(rotmatx, v);
}

inline point3d RotateY(point3d v, double angle)
{
    double c = cos(angle * pi / 180);
    double s = sin(angle * pi / 180);

    mat3d rotmaty = { c, 0, s,
                      0, 1, 0,
                     -s, 0, c };

    return matMult(rotmaty, v);
}

inline point3d RotateZ(point3d v, double angle)
{
    double c = cos(angle * pi / 180);
    double s = sin(angle * pi / 180);

    mat3d rotmatz = { c, -s, 0,
                      s, c, 0,
                      0, 0, 1 };

    return matMult(rotmatz, v);
}


// This should go in a different file, but for now I'm just putting it here
inline double dotProduct(point3d a, point3d b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double norm3d(point3d a)
{
    return sqrt(dotProduct(a, a));
}

inline point3d crossProduct(point3d a, point3d b)
{
    point3d crossOut(0, 0, 0);

    crossOut.x = a.y * b.z - a.z * b.y;
    crossOut.y = -(a.x * b.z - a.z * b.x);
    crossOut.z = a.x * b.y - a.y * b.x;

    return crossOut;
}

struct plane2d
{
    double A, B, C, D;
};

struct point2d
{
    double U, V;
};

struct color3
{
    double r, g, b;
};

struct sphere3d
{
    point3d center;
    double radius;
};

struct triangle3d
{
    point3d normal;
    point3d a, b, c;
};

struct polygon3d
{
    point3d normal;
    std::vector<point3d> vertices;
};

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
        color3 mColor;

    private:
        point3d SphereIntersection(const point3d& rayOrigin, const point3d& rayDirection);
        point3d TriangleIntersection(const point3d& rayOrigin, const point3d& rayDirection);
        point3d PolygonIntersection(const point3d& rayOrigin, const point3d& rayDirection);

    private:
        WorldObjectType mShapeType;
};
