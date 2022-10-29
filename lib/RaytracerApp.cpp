#include "RaytracerApp.h"
#include "Camera.h"
#include "Viewport.h"
#include <iostream>

// Raytracer App Constructor
RaytracerApp::RaytracerApp()
{
    AppRunning = true;

}

int RaytracerApp::RunApp()
{
    Camera camera(GuiApp.GetHeight(), GuiApp.GetWidth(), GuiApp);

    CameraAction action = CameraAction::None;
    double illuminationPercentage = 1;

    while (AppRunning)
    {
        // get events from GUIApp
        GuiApp.PollEvents();

        // render
        // Basically this is going to be split into the camera and the world
        // Camera will include the world, but will also interact

        GuiApp.UpdateFrame();
        GuiApp.UpdateGui();

        action = GuiApp.GetGuiAction();
        illuminationPercentage = GuiApp.GetIlluminationPercentage();
        camera.SetIlluminationPercentage(illuminationPercentage);

        // Should add way to pass data between
        camera.DoCameraAction(action);

        // Reset state, could probably set up a queue, but that's beyond the scope of this project
        action = CameraAction::None;

        // Check if we should terminate the app
        AppRunning = !GuiApp.GetWindowShouldClose();
    }

    return 0;
}
