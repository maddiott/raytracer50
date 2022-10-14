#pragma once

#include "Viewport.h"

#include <iostream>
#include <random>

typedef struct
{
    double x;
    double y;
    double z;
} point3d;

class Camera
{
    public:
        Camera(int height, int width, Viewport& canvas);

        void SetWidth(int width);
        int GetWidth();

        void SetHeight(int height);
        int GetHeight();

        void SetCanvas(Viewport& canvas);

        void Render();
        void RenderSpheres(int NumSpheres);

    private:
        int mHeight;
        int mWidth;
        int mFrameCount;

        // Want a pointer to the viewport object
        Viewport &mCanvas;

        std::default_random_engine generator;
        std::uniform_int_distribution<unsigned int> distribution;


};
