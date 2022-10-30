#include "Camera.h"
#include "CameraMessage.h"

#include <iostream>
#include <numbers>
#include <vector>


Camera::Camera(int height, int width, Viewport& canvas) :
    mHeight(480),
    mWidth(640),
    mCanvas(canvas),
    mPixelCoords(mHeight, std::vector<point3d>(mWidth, point3d(0, 0, 0))),
    mIlluminationPercentage(1.0),
    mIlluminationPercentageToRender(1.0),
    mAngle(0.0)
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

    // Looks like superscalar is working in my favor, this is more threads than I have cpu cores and I think it's a local min in terms of execution time
    // It seems like we get major stability issues with threads > cores, so maybe don't
    auto availableCores = std::thread::hardware_concurrency();
    mRenderThreads = 6;

    if (mRenderThreads > (availableCores / 2))
    {
        std::cout << "Warning " << mRenderThreads << " render threads is greater than " << availableCores / 2 << " physical cores\n";
        std::cout << "Program may be unstable\n\n\n\n";
    }

    mACounter = mRenderThreads;

    isRunning = true;
    for (int i = 1; i <= mRenderThreads; i++)
    {
        std::thread t(&Camera::RenderWorld, this, i, mRenderThreads);
        mRenderThreadPool.push_back(std::move(t));
    }

    std::cout << "Thread pool size: " << mRenderThreadPool.size() << '\n';

    isClearing = false;
}

Camera::~Camera()
{
    for (auto& t : mRenderThreadPool)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
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

void Camera::SetIlluminationPercentage(double illuminationPercentage)
{
    if (abs(illuminationPercentage - mIlluminationPercentage) > 0.05)
    {
        mIlluminationQueue.push(illuminationPercentage);
    }
    mIlluminationPercentage = illuminationPercentage;
}

double Camera::GetIlluminationPercentage()
{
    return mIlluminationPercentage;
}

void Camera::SetCanvas(Viewport& canvas)
{
}

void Camera::ClearCanvas(Viewport& canvas)
{
    isClearing = true;
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

void Camera::DoCameraAction(CameraAction action, CameraMessage cameraMsg)
{
    SetIlluminationPercentage(cameraMsg.mIlluminationPercentage);

    switch (action)
    {
     // We're going to empty the illumination queue if we have no action to perform
    case CameraAction::None:
        if ((!isRendering) && (mIlluminationQueue.size() > 0))
        {
            mACounter = 0;
            if (mIlluminationQueue.size() > 0)
            {
                mIlluminationPercentageToRender = mIlluminationQueue.front();
                mIlluminationQueue.pop();
                mStartTime = std::chrono::system_clock::now();
                if (mACounter == 0)
                {
                    isRendering = true;
                }
            }
        }
        break;
    case CameraAction::DrawWorld:
        // Action queue helps with state sync between gui and renderer
        if (isRendering == false)
        {
            mACleanCounter = 0;
            ClearCanvas(mCanvas);
            mIlluminationPercentageToRender = mIlluminationPercentage;
            mACounter = 0;
            mStartTime = std::chrono::system_clock::now();
            if (mACounter == 0)
            {
                isRendering = true;
            }
        }
        break;
    case CameraAction::DrawGradient:
        RenderGradient();
        break;
    case CameraAction::SliderChanged:
        break;
    case CameraAction::LoadObj:
        std::cout << "Hello from camera, obj is: " << cameraMsg.mObjFilepath << '\n';
        mWorld.LoadObject(cameraMsg.mObjFilepath);
        break;
    case CameraAction::StopRender:
        isRunning = false;
        break;
    case CameraAction::RotateWorld:
        if (isRendering == false)
        {
            mACleanCounter = 0;
            ClearCanvas(mCanvas);
            mWorld.ApplyTransformation(point3d(0, 0, 10), 45, mAngle, 0);
            mAngle += 5;
            mIlluminationPercentageToRender = mIlluminationPercentage;
            mACounter = 0;
            mStartTime = std::chrono::system_clock::now();
            if (mACounter == 0)
            {
                isRendering = true;
            }
        }
        break;
    case CameraAction::ClearCanavas:
        if (isRendering == false)
        {
            mACleanCounter = 0;
            ClearCanvas(mCanvas);
        }
        break;
    default:
        break;
    }

}

void Camera::RenderWorld(int ThreadNumber, int NumThreads)
{
    point3d RayNormalVector;
    double magnitude = 0;
    int startingLine = (mHeight / NumThreads) * ThreadNumber - 1;
    int endingLine = (mHeight / NumThreads) * (ThreadNumber - 1);

    double illuminationPercentage;
    
    while (isRunning)
    {
        if ((isClearing) && (mACleanCounter < mRenderThreads))
        {
            for (int i = startingLine; i >= endingLine; i--)
            {
                for (int j = 0; j < mWidth; j++)
                {
                    mCanvas.WritePixel(j,
                        i,
                        (GLubyte) 0,
                        (GLubyte) 0,
                        (GLubyte) 0);
                }
            }

            int threadCount;
            threadCount = ++mACleanCounter;

            // Spin wait until all of the threads have finished
            // If we don't do this the loop continues and we have a race condition
            while (mACleanCounter != mRenderThreads)
            {
            }

            if (threadCount == mRenderThreads)
            {
                isClearing = false;

                std::cout << "Canvas cleared\n";
            }

        }
        else if ((isRendering) && (mACounter < mRenderThreads))
        {
            illuminationPercentage = mIlluminationPercentageToRender;
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
                        point3d illuminationDirection(0, -(1 - illuminationPercentage), 1);

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

            int threadCount = 0;
            threadCount = mACounter;

            //std::cout << "Thread " << threadCount << '\n';

            // Spin wait until all of the threads have finished
            // If we don't do this the loop continues and we have a race condition
            while (mACounter < mRenderThreads)
            { }

            
            if (threadCount == mRenderThreads)
            {
                isRendering = false;
                mEndTime = std::chrono::system_clock::now();

                std::cout <<
                    "Render duration: "
                    << std::chrono::duration_cast<std::chrono::milliseconds> (mEndTime - mStartTime).count() / 1000.0
                    << " S\n";
            }
            
        }

        // If the render flag isn't reset on every thread, we can wind up in a situation where the rendering loop is re entered
        if (mACounter == mRenderThreads)
        {
            isRendering = false;
        }
        // Looks like there's a a state where it pause, might be an allocation thing or one of the atomics blocking
    }
}

