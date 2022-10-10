#include "RaytracerApp.h"
#include "Viewport.h"
#include <iostream>

// Raytracer App Constructor
RaytracerApp::RaytracerApp()
{
    AppRunning = true;

}

int RaytracerApp::RunApp()
{
    while (AppRunning)
    {
        // get events from GUIApp
        GuiApp.PollEvents();

        // render
        // Basically this is going to be split into the camera and the world
        // Camera will include the world, but will also interact

        // update canvas
        GuiApp.UpdateFrame();

        // Check if we should terminate the app
        AppRunning = !GuiApp.GetWindowShouldClose();
    }

    return 0;
}
