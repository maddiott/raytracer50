#pragma once
// This where the subcomponents of the app are called
// The main render loop is here

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