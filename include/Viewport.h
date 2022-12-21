// This file defines the OpenGl Canvas Class
// It also acts as a wrapper around the dear imgui example code
// The gui logic is implemented here as well and is decoupled from the
// Renderer logic
#pragma once

#include "CameraAction.h"
#include "CameraMessage.h"
#include "RtMathHelp.h"
 
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <chrono>
#include <memory>
#include <vector>
#include <string>

class Viewport
{
    public:
        Viewport(int width, int height);
        Viewport() : Viewport(640, 480) {};

        ~Viewport();

        int GetWidth();
        void SetWidth(int width);

        int GetHeight();
        void SetHeight(int height);

        double GetIlluminationPercentage();

        void WritePixel(int x, int y, GLubyte r, GLubyte g, GLubyte b);
        int WriteFrame(const std::string &Filename);

        void UpdateFrame();
        CameraMessage UpdateGui();

        void PollEvents();

        void ClearCanvas();

        bool GetWindowShouldClose();

        CameraAction GetGuiAction();


    // Private functions
    private:
        // c callbacks are fun in c++ classes
        // This is just a way of making it so that GLFW (OpenGl)
        // callbacks are available in c++ classes
        //https://stackoverflow.com/questions/44711290/passing-in-c-method-as-a-function-pointer
        void bind();
        
        // key callback for GLFW
        void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void KeyCallbackStatic(GLFWwindow* window, int key, int scancode, int action, int mods);

    // Private variables
    private:
        bool windowShouldClose;
        GLFWwindow* AppWindow;

        int width = 640;
        int height = 480;

        ImGuiIO* io;

        // Actions pertaining to window state are internal, but we want a way to talk to the renderer
        int action = 0;

        std::vector<GLubyte> canvas;
        char mFilePath[128];
        char mFilePathObj[128];

        CameraAction ActionReturned;
        
        // This should be a smart pointer, but the cleanup code is included from the imgui example
        ImGuiContext* GuiContext;

        // Dynamic thread 
        int mNumRenderThreads;

        double mIlluminationPercentage;
        double mXAngle;
        double mYAngle;
        double mZAngle;

        point3d mTranslation;

        // 
        bool mAnimate;

        // Render timer
        std::chrono::time_point<std::chrono::system_clock> mStartTime;
        std::chrono::time_point<std::chrono::system_clock> mEndTime;
};

