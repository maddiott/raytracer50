#include "Camera.h"
#include <iostream>
#include <numbers>
#include <thread>
#include <vector>


Camera::Camera(int height, int width, Viewport &canvas) : mHeight(480), mWidth(640), mCanvas(canvas), mPixelCoords(mHeight, std::vector<point3d>(mWidth, point3d(0, 0, 0)))
{
	SetWidth(width);
	SetHeight(height);
    mFrameCount = 0;

    // To randomize color and later sampling
    std::uniform_int_distribution<unsigned int> distribution(0, 255);
    isRendering = false;

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

    for (int i = 0; i < mHeight; i++)
    {
        for (int j = 0; j < mWidth; j++)
        {
            mPixelCoords[i][j].x = j * xStep + xStep / 2 + xMin;
            mPixelCoords[i][j].y = i * yStep + yStep / 2 + yMin;
            mPixelCoords[i][j].z = mCamera[2];
        }
    }

    MakeSpheres(50);

    mACounter = 0;

    // Looks like superscalar is working in my favor, this is more threads than I have cpu cores and I think it's a local min in terms of execution time
    mRenderThreads = 6 * 2;
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

// Gradient test render
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
        if (!isRendering)
        {
            MakeSpheres(50);
            isRendering = true;
            mACounter = 0;
            mStartTime = std::chrono::system_clock::now();
            for (int i = 1; i <= mRenderThreads; i++)
            {
                std::thread t(&Camera::RenderSpheres, this, i, mRenderThreads);
                t.detach();
            }
        }
        break;
    default:
        break;
    }

}

void Camera::MakeSpheres(int NumSpheres)
{
    // reset mSpheres
    mSpheres = std::vector<sphere3d> {};

    // this will be a sphere making thing
    // Start with sphere world coord x, y, z
    double centX, centY, centZ;

    double radius;
    sphere3d sphereTemp;

    std::uniform_real_distribution<double> distributionCent(-7, 7);
    std::uniform_real_distribution<double> distributionRadius(0.05, 1);

    for (int i = 0; i < NumSpheres; i++)
    {
        centX = ((double)distributionCent(generator));
        centY = ((double)distributionCent(generator));
        centZ = 10;// distributionCent(generator);

        GLubyte r = (GLubyte)distribution(generator);
        GLubyte g = (GLubyte)distribution(generator);
        GLubyte b = (GLubyte)distribution(generator);

        radius = distributionRadius(generator);

        sphereTemp.center = point3d(centX, centY, centZ);
        sphereTemp.color = point3d(r, g, b);
        sphereTemp.radius = radius;

        mSpheres.push_back(sphereTemp);
    }
    // end of sphere construction

}

void Camera::RenderSpheres(int ThreadNumber, int NumThreads)
{
    point3d normVectorPix;
    double magnitude = 0;
    int startingLine = (mHeight / NumThreads) * ThreadNumber - 1;
    int endingLine = (mHeight / NumThreads) * (ThreadNumber - 1);

    for (int i = startingLine; i >= endingLine; i--)
    {
        for (int j = 0; j < mWidth; j++)
        {
            for (const sphere3d& sphere : mSpheres)
            {
                // Figure out direction (normalize vector)
                magnitude = sqrt((mPixelCoords[i][j].x * mPixelCoords[i][j].x)
                    + (mPixelCoords[i][j].y * mPixelCoords[i][j].y)
                    + (mPixelCoords[i][j].z * mPixelCoords[i][j].z));

                normVectorPix.x = mPixelCoords[i][j].x / magnitude;
                normVectorPix.y = mPixelCoords[i][j].y / magnitude;
                normVectorPix.z = mPixelCoords[i][j].z / magnitude;

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
                double B = 2 * ((normVectorPix.x * (-sphere.center.x))
                    + (normVectorPix.y * (-sphere.center.y))
                    + (normVectorPix.z * (-sphere.center.z)));

                double C = (-sphere.center.x) * (-sphere.center.x)
                    + (-sphere.center.y) * (-sphere.center.y)
                    + (-sphere.center.z) * (-sphere.center.z) - (sphere.radius * sphere.radius);

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
                normal.x = (intersectVector.x - sphere.center.x) / sphere.radius;
                normal.y = (intersectVector.y - sphere.center.y) / sphere.radius;
                normal.z = (intersectVector.z - sphere.center.z) / sphere.radius;

                // Draw

                // Find the lamberCosine for shading
                double lambertCosine = normVectorPix.x * normal.x + normVectorPix.y * normal.y + normVectorPix.z * normal.z;
                point3d colorToDraw;

                colorToDraw.x = sphere.color.x * abs(lambertCosine);
                colorToDraw.y = sphere.color.y * abs(lambertCosine);
                colorToDraw.z = sphere.color.z * abs(lambertCosine);

                mCanvas.WritePixel(j,
                    i,
                    (GLubyte)colorToDraw.x,
                    (GLubyte)colorToDraw.y,
                    (GLubyte)colorToDraw.z);
            }
        }
    }

    mACounter++;
    if (mACounter == mRenderThreads)
    {
        mEndTime = std::chrono::system_clock::now();
        isRendering = false;

        std::cout <<
            "Render duration: "
            << std::chrono::duration_cast<std::chrono::milliseconds> (mEndTime - mStartTime).count() / 1000.0
            << " S\n";
    }
}

