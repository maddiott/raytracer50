#pragma once

#include "Viewport.h"
#include "World.h"

#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

/*typedef struct
{
    double x;
    double y;
    double z;
} point3d;

typedef struct
{
    point3d center;
    point3d color;
    double radius;
} sphere3d;*/

class Camera
{
    public:
        Camera(int height, int width, Viewport& canvas);

        void SetWidth(int width);
        int GetWidth();

        void SetHeight(int height);
        int GetHeight();

        void SetCanvas(Viewport& canvas);

        void DoCameraAction(CameraAction action);

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
        void MakeSpheres(int NumSpheres);

        void Render();
        void RenderSpheres(int ThreadNumber, int NumThreads);

        int mRenderThreads;

        point3d mCameraOrigin = { 0, 0, 0 };
        std::vector<std::vector<point3d>> mPixelCoords;

        std::vector<sphere3d> mSpheres;

        // Decided to try out atomics to help with detached thread synchronization
        std::atomic<int> mACounter;

        // Adding some benchmarking using std chrono
        // Idea from https://gist.github.com/mcleary/b0bf4fa88830ff7c882d
        std::chrono::time_point<std::chrono::system_clock> mStartTime;
        std::chrono::time_point<std::chrono::system_clock> mEndTime;

        bool isRendering;


};

