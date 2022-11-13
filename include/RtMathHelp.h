#pragma once

/* Math helper library for basic vector operations
  All functions are inlined due to not wanting to incur function call overhead
  This might be a wash due to instruction cache utilization, but I believe this
  workload is compute bound
*/

#include <cmath>
#include <vector>

constexpr double pi = 3.14159265;

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

struct colorRgb
{
    double r, g, b;
};

inline colorRgb operator*(double b, const colorRgb& a)
{
    colorRgb result(0, 0, 0);

    result.r = b * a.r;
    result.g = b * a.g;
    result.b = b * a.b;

    return result;
}

struct colorHsv
{
    double h, s, v;
};

// Formula from https://www.rapidtables.com/convert/color/rgb-to-hsv.html
inline colorHsv rgbToHsv(colorRgb colorIn)
{
    colorHsv colorOut(0, 0, 0);

    double sR = colorIn.r / 255;
    double sG = colorIn.g / 255;
    double sB = colorIn.b / 255;

    double cMax, cMin, delta;

    if ((sR <= sG) && (sR <= sB))
    {
        cMin = sR;
    }
    else if ((sG <= sR) && (sG <= sB))
    {
        cMin = sG;
    }
    else
    {
        cMin = sB;
    }

    if ((sR >= sG) && (sR >= sB))
    {
        cMax = sR;

    }
    else if ((sG >= sR) && (sG >= sB))
    {
        cMax = sG;
    }
    else
    {
        cMax = sB;
    }

    delta = cMax - cMin;

    if (delta == 0)
    {
        colorOut.h = 0;
    }
    else if (cMax == sR)
    {
        colorOut.h = 60 * fmod(((sG - sB) / delta), 6);
    }
    else if (cMax == sG)
    {
        colorOut.h = 60 * (((sB - sR) / delta) + 2);
    }
    else
    {
        colorOut.h = 60 * (((sR - sG) / delta) + 4);
    }

    colorOut.s = (cMax == 0) ? 0 : delta / cMax;

    colorOut.v = cMax;

    return colorOut;
}

// https://www.rapidtables.com/convert/color/hsv-to-rgb.html
inline colorRgb hsvToRgb(colorHsv colorIn)
{
    colorRgb colorOut;

    double c = colorIn.v * colorIn.s;
    double x = c * (1
        - abs(fmod((colorIn.h / 60), 2) - 1));
    double m = colorIn.v - c;

    if ((colorIn.h <= 0) && (colorIn.h < 60))
    {
        colorOut.r = c;
        colorOut.g = x;
        colorOut.b = 0;
    }
    else if ((colorIn.h <= 60) && (colorIn.h < 120))
    {
        colorOut.r = x;
        colorOut.g = c;
        colorOut.b = 0;
    }
    else if ((colorIn.h <= 120) && (colorIn.h < 180))
    {
        colorOut.r = 0;
        colorOut.g = c;
        colorOut.b = x;
    }
    else if ((colorIn.h <= 180) && (colorIn.h < 240))
    {
        colorOut.r = 0;
        colorOut.g = x;
        colorOut.b = c;
    }
    else if ((colorIn.h <= 240) && (colorIn.h < 300))
    {
        colorOut.r = x;
        colorOut.g = 0;
        colorOut.b = c;
    }
    else
    {
        colorOut.r = c;
        colorOut.g = 0;
        colorOut.b = x;
    }

    colorOut.r = (colorOut.r + m) * 255;
    colorOut.g = (colorOut.g + m) * 255;
    colorOut.b = (colorOut.b + m) * 255;

    return colorOut;
}


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
    point3d normal = point3d(0, 0, 0);
    std::vector<point3d> vertices;
};
