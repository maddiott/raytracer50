#pragma once

#include "Viewport.h"

#include <iostream>

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

    private:
        int mHeight;
        int mWidth;
        int mFrameCount;

        // Want a pointer to the viewport object
        Viewport &mCanvas;
        
};
