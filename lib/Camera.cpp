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

    // tan(yFov) = yMin / zToCam
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

    //mWorld.LoadSpheres(10);
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
void Camera::RenderGradient()
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

void Camera::DoCameraAction(CameraAction action, double illuminationPercentage)
{
    switch (action)
    {
    case CameraAction::None:
        break;
    case CameraAction::DrawWorld:
        if (!isRendering)
        {
            isRendering = true;
            mACounter = 0;
            mStartTime = std::chrono::system_clock::now();
            for (int i = 1; i <= mRenderThreads; i++)
            {
                std::thread t(&Camera::RenderWorld, this, i, mRenderThreads, illuminationPercentage);
                t.detach();
            }
        }
        break;
    case CameraAction::DrawGradient:
        RenderGradient();
        break;
    default:
        break;
    }

}

void Camera::RenderWorld(int ThreadNumber, int NumThreads, double illuminationPercentage)
{
    point3d RayNormalVector;
    double magnitude = 0;
    int startingLine = (mHeight / NumThreads) * ThreadNumber - 1;
    int endingLine = (mHeight / NumThreads) * (ThreadNumber - 1);

    constexpr double pi = 3.14159265;

    constexpr double angle = 45;
    double c = cos(angle * pi / 180);
    double s = sin(angle * pi / 180);

    mat3d rotmatx = { 1, 0, 0,
                      0, c, -s,
                      0, s, c };

    mat3d rotmaty = { c, 0, s,
                      0, 1, 0,
                     -s, 0, c };

    mat3d rotmatz = { c, -s, 0,
                      s, c, 0,
                      0, 0, 1 };

    mat3d eye = { 1, 0, 0,
                  0, 1, 0,
                  0, 0, 1 };

    mCameraOrigin = matMult(eye, mCameraOrigin);


    for (int i = startingLine; i >= endingLine; i--)
    {
        for (int j = 0; j < mWidth; j++)
        {
                // Figure out direction (normalize vector)
            magnitude = norm3d(mPixelCoords[i][j]);

                RayNormalVector = mPixelCoords[i][j] / magnitude;

                // See if the ray hits anything
                color3 color(0, 0, 0);
                point3d normal(0, 0, 0);

                mWorld.TestIntersection(mCameraOrigin, RayNormalVector, normal, color);

                // Draw if we got a hit
                if (norm3d(normal) > 0)
                {
                    // Find the lamberCosine for shading
                    point3d illuminationOrigin(0, 5, 0);
                    point3d illuminationDirection(0,  - (1 - illuminationPercentage), 1);

                    point3d illuminationNormal;
                    illuminationDirection = illuminationDirection / norm3d(illuminationDirection);
                    mWorld.TestIntersection(illuminationOrigin, illuminationDirection, illuminationNormal, color);

                    double lambertCosine = dotProduct(normal, illuminationDirection);
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

