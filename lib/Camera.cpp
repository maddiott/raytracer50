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
    std::uniform_int_distribution<unsigned int> mDistribution(0, 255);
    isRendering = false;

    // Fov in degrees
    double yFov = (std::numbers::pi * 30) / 180;
    double xFov = (std::numbers::pi * 40) / 180;

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
            mPixelCoords[i][j].z =  1;
        }
    }

    mWorld.LoadSpheres(10);
    mWorld.LoadCube();

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
    /*/ reset mSpheres
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
        centX = ((double)distributionCent(mGenerator));
        centY = ((double)distributionCent(mGenerator));
        centZ = 10;// distributionCent(generator);

        GLubyte r = (GLubyte)mDistribution(mGenerator);
        GLubyte g = (GLubyte)mDistribution(mGenerator);
        GLubyte b = (GLubyte)mDistribution(mGenerator);

        radius = distributionRadius(mGenerator);

        sphereTemp.center = point3d(centX, centY, centZ);
        sphereTemp.color = point3d(r, g, b);
        sphereTemp.radius = radius;

        mSpheres.push_back(sphereTemp);
    }
    // end of sphere construction*/

}

void Camera::RenderSpheres(int ThreadNumber, int NumThreads)
{
    point3d RayNormalVector;
    double magnitude = 0;
    int startingLine = (mHeight / NumThreads) * ThreadNumber - 1;
    int endingLine = (mHeight / NumThreads) * (ThreadNumber - 1);

    for (int i = startingLine; i >= endingLine; i--)
    {
        for (int j = 0; j < mWidth; j++)
        {
                // Figure out direction (normalize vector)
            magnitude = norm3d(mPixelCoords[i][j]);/*sqrt((mPixelCoords[i][j].x * mPixelCoords[i][j].x)
                    + (mPixelCoords[i][j].y * mPixelCoords[i][j].y)
                    + (mPixelCoords[i][j].z * mPixelCoords[i][j].z))*/;

                /*RayNormalVector.x = mPixelCoords[i][j].x / magnitude;
                RayNormalVector.y = mPixelCoords[i][j].y / magnitude;
                RayNormalVector.z = mPixelCoords[i][j].z / magnitude;*/

                RayNormalVector = mPixelCoords[i][j] / magnitude;

                // See if the ray hits anything
                color3 color(0, 0, 0);
                point3d normal(0, 0, 0);
                if ((i == 220) && (j == 320))
                {
                    int a = 1;
                }
                mWorld.TestIntersection(mCameraOrigin, RayNormalVector, normal, color);

                // Draw
                if (norm3d(normal) > 0)
                {
                    // Find the lamberCosine for shading
                    double lambertCosine = RayNormalVector.x* normal.x + RayNormalVector.y * normal.y + RayNormalVector.z * normal.z;
                    point3d colorToDraw;

                    colorToDraw.x = color.r * abs(lambertCosine);
                    colorToDraw.y = color.g * abs(lambertCosine);
                    colorToDraw.z = color.b * abs(lambertCosine);

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

