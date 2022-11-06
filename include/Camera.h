#pragma once

#include "RtMathHelp.h"
#include "Viewport.h"
#include "World.h"

#include <atomic>
#include <chrono>
#include <iostream>
#include <queue>
#include <random>
#include <thread>
#include <vector>

class Camera
{
    public:
        Camera(int height, int width, Viewport& canvas, int numThreads);
        ~Camera();

        void SetWidth(int width);
        int GetWidth();

        void SetHeight(int height);
        int GetHeight();

        void SetIlluminationPercentage(double illuminationPercentage);
        double GetIlluminationPercentage();

        void SetCanvas(Viewport& canvas);
        void ClearCanvas(Viewport& canvas);

        void DoCameraAction(CameraAction action, CameraMessage cameraMsg);

        void SetRenderThreads(int RenderThreads);

    private:
        int mHeight;
        int mWidth;
        int mFrameCount;

        // Want a pointer to the viewport object
        Viewport &mCanvas;
        World mWorld;


        std::default_random_engine mGenerator;
        std::uniform_int_distribution<unsigned int> mDistribution;


    private:
        void RenderGradient();
        void RenderWorld(int ThreadNumber, int NumThreads);

        point3d mCameraOrigin = { 0, 0, 0 };
        std::vector<std::vector<point3d>> mPixelCoords;

        std::vector<sphere3d> mSpheres;

        // Decided to try out atomics to help with detached thread synchronization
        std::atomic<int> mACounter;
        std::atomic<int> mACleanCounter;

        // Adding some benchmarking using std chrono
        // Idea from https://gist.github.com/mcleary/b0bf4fa88830ff7c882d
        std::chrono::time_point<std::chrono::system_clock> mStartTime;
        std::chrono::time_point<std::chrono::system_clock> mEndTime;

        bool isRunning;
        std::atomic<bool> isRendering;
        std::atomic<bool> isClearing;

        // Threads
        int mRenderThreads;
        std::vector<std::thread> mRenderThreadPool;

        double mIlluminationPercentage;
        std::atomic<double> mIlluminationPercentageToRender;

        std::queue<double> mIlluminationQueue;

        double mAngle;
        CameraMessage mCameraState;
};

