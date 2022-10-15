#pragma once

#include "Viewport.h"

class RaytracerApp
{
    public:
        RaytracerApp();
        //~RaytracerApp();

        int RunApp();

    private:
        // By declaring Viewport here this gets automatically constructed when RaytracerApp gets constructed, neat
        Viewport GuiApp;
        bool AppRunning;
};