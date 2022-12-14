#include "RaytracerApp.h"
#include "Camera.h"
#include "CameraMessage.h"

#include "Viewport.h"
#include <iostream>

// Raytracer App Constructor
RaytracerApp::RaytracerApp()
{
    AppRunning = true;

}

int RaytracerApp::RunApp()
{
    Camera camera(GuiApp.GetHeight(), GuiApp.GetWidth(), GuiApp, 1);

    CameraAction action = CameraAction::None;
    double illuminationPercentage = 1;

    CameraMessage cameraMsg;

    bool breakStuff = false;

    while (AppRunning)
    {
        // get events from GUIApp
        GuiApp.PollEvents();

        // render
        // Basically this is going to be split into the camera and the world
        // Camera will include the world, but will also interact

        GuiApp.UpdateFrame();
        cameraMsg = GuiApp.UpdateGui();

        // Should add way to pass data between
        action = GuiApp.GetGuiAction();

        if (action == CameraAction::StopRender)
        {
            camera.DoCameraAction(action, cameraMsg);
        }
        else
        {
            camera.DoCameraAction(action, cameraMsg);
        }

        // Reset state, could probably set up a queue, but that's beyond the scope of this project
        action = CameraAction::None;

        // Check if we should terminate the app
        AppRunning = !GuiApp.GetWindowShouldClose();
    }
    std::cout << "Window should close\n";

    // If we close the app from the OpenGl context, the program will hang
    // To solve this we just tell the camera to stop rendering
    action = CameraAction::StopRender;
    camera.DoCameraAction(action, cameraMsg);

    return 0;
}
