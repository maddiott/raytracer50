#include "Camera.h"
#include <iostream>
#include <numbers>
#include <vector>


Camera::Camera(int height, int width, Viewport &canvas) : mHeight(480), mWidth(640), mCanvas(canvas)
{
	SetWidth(width);
	SetHeight(height);
    mFrameCount = 0;

    // To randomize color and later sampling
    std::uniform_int_distribution<unsigned int> distribution(0, 255);
}

void Camera::SetWidth(int width)
{
	mWidth = width;
}

int Camera::GetWidth()
{
	return mWidth;
}

void Camera::SetHeight(int height)
{
	mHeight = height;
}

int Camera::GetHeight()
{
	return mHeight;
}

void Camera::SetCanvas(Viewport& canvas)
{
}

void Camera::Render()
{
    int rOffset = (mFrameCount % 480);
    int gOffset = ((mFrameCount >> 8) % 480);
    int bOffset = ((mFrameCount >> 16) % 480);

    float r = 0;
    float g = 0;
    float b = 0;

    for (int i = 0; i < mHeight; i++)
    {
        for (int j = 0; j < mWidth; j++)
        {
            r = (GLubyte)((((float) (i + rOffset)) / ((float)mHeight)) * 255);
            g = (GLubyte)((((float) (i + j + gOffset)) / ((float)(mHeight + mWidth))) * 255);
            b = (GLubyte)((((float) (j + bOffset)) / ((float)mWidth)) * 255);

            mCanvas.WritePixel(j, i, r, g, b);
        }
    }

    mFrameCount++;
}

void Camera::DoCameraAction(CameraAction action)
{
    switch (action)
    {
        case CameraAction::None:
            break;
        case CameraAction::DrawSphere:
            RenderSpheres(5);
            break;
        default:
            break;
    }

}

void Camera::RenderSpheres(int NumSpheres)
{
    // Start with sphere world coord x, y, z
    std::vector<point3d> spheres;
    double centX, centY, centZ;

    std::uniform_int_distribution<unsigned int> distributionCent(0, 10);

    for (int i = 0; i < NumSpheres; i++)
    {
        centX = ((double)distributionCent(generator)) - 5;
        centY = ((double)distributionCent(generator)) - 5;
        centZ = 10;// distributionCent(generator);

       
        spheres.push_back(point3d(centX, centY, centZ));
        /*spheres.push_back(point3d(0, 0, 10));
        spheres.push_back(point3d(2, 0, 2));
        spheres.push_back(point3d(0, 2, 4));*/
        //spheres.push_back(point3d(-3, 0, 10));
    }

    double radius = 1;

    // Camera world coord
    std::vector<double> camera = { 0, 0, 1 };

    // Fov in degrees
    double yFov = (std::numbers::pi * 60) / 180;
    double xFov = (std::numbers::pi * 60) / 180;

    // tan(xFov) = xMin / zToCam
    double xMax = tan(xFov / 2);
    double xMin = -xMax;
    double xStep = (xMax - xMin) / mWidth;

    // tan(xFov) = xMin / zToCam
    double yMax = tan(yFov / 2);
    double yMin = -yMax;
    double yStep = (yMax - yMin) / mHeight;

    std::vector<std::vector<point3d>> pixelCoords(mHeight, std::vector<point3d>(mWidth, point3d(0, 0, 0)));

    for (int i = 0; i < mHeight; i++)
    {
        for (int j = 0; j < mWidth; j++)
        {
            pixelCoords[i][j].x = j * xStep + xStep / 2 + xMin;
            pixelCoords[i][j].y = i * yStep + yStep / 2 + yMin;
            pixelCoords[i][j].z = camera[2];
        }
    }


    point3d normVectorPix;
    double magnitude = 0;
    for (const point3d &center : spheres)
    {
        //std::cout << "Center is (" << center.x << ',' << center.y << ',' << center.z << ")\n";
        GLubyte r = (GLubyte)distribution(generator);
        GLubyte g = (GLubyte)distribution(generator);
        GLubyte b = (GLubyte)distribution(generator);

        point3d color = { r, g, b };
        for (int i = 0; i < mHeight; i++)
        {
            for (int j = 0; j < mWidth; j++)
            {
                // Figure out direction (normalize vector)
                magnitude = sqrt((pixelCoords[i][j].x * pixelCoords[i][j].x)
                    + (pixelCoords[i][j].y * pixelCoords[i][j].y)
                    + (pixelCoords[i][j].z * pixelCoords[i][j].z));

                normVectorPix.x = pixelCoords[i][j].x / magnitude;
                normVectorPix.y = pixelCoords[i][j].y / magnitude;
                normVectorPix.z = pixelCoords[i][j].z / magnitude;

                // Send out ray (see Glassner chapter 2, this is the algebraic form of the parametric intersection)
                double A = normVectorPix.x * normVectorPix.x
                    + normVectorPix.y * normVectorPix.y
                    + normVectorPix.z * normVectorPix.z;
                // inefficient since branch, but we'll get rid of this later
                if (abs(A - 1) > 0.0001)
                {
                    std::cout << "might want to take a look at this, magnitude is substantially different than 1 "
                        << A << '\n';
                }

                // Origin is defined as 0 so X_o = Y_o = Z_o = 0
                double B = 2 * ((normVectorPix.x * (-center.x))
                    + (normVectorPix.y * (-center.y))
                    + (normVectorPix.z * (-center.z)));

                double C = (-center.x) * (-center.x)
                    + (-center.y) * (-center.y)
                    + (-center.z) * (-center.z) - (radius * radius);

                // Check intersect
                double discriminant = B * B - 4 * A * C;
                if (discriminant < 0)
                {
                    // If discriminant is less than zero we miss
                    continue;
                }

                double t0 = (-B - sqrt(discriminant)) / 2;
                double t1 = (-B + sqrt(discriminant)) / 2;

                // Closest hit is the one we care about
                double distance = (t0 < t1) ? t0 : t1;

                point3d intersectVector;
                intersectVector.x = distance * normVectorPix.x;
                intersectVector.y = distance * normVectorPix.y;
                intersectVector.z = distance * normVectorPix.z;

                // Check normal
                point3d normal;
                normal.x = (intersectVector.x - center.x) / radius;
                normal.y = (intersectVector.y - center.y) / radius;
                normal.z = (intersectVector.z - center.z) / radius;

                // Draw

                // Find the lamberCosine for shading
                double lambertCosine = normVectorPix.x * normal.x + normVectorPix.y * normal.y + normVectorPix.z * normal.z;
                point3d colorToDraw;

                colorToDraw.x = color.x * abs(lambertCosine);
                colorToDraw.y = color.y * abs(lambertCosine);
                colorToDraw.z = color.z * abs(lambertCosine);

                mCanvas.WritePixel(j,
                    i,
                    (GLubyte)colorToDraw.x,
                    (GLubyte)colorToDraw.y,
                    (GLubyte)colorToDraw.z);
            }
        }
    }
}

