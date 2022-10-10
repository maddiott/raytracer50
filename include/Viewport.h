// This file defines the OpenGl Canvas Class
#pragma once

#include <GLFW/glfw3.h>
#include <vector>

class Viewport
{
    public:
        Viewport(int width, int height, int channels);
        Viewport() : Viewport(640, 480, 3) {};

        ~Viewport();

        int GetWidth();
        void SetWidth(int width);

        int GetHeight();
        void SetHeight(int height);

        // For now channels will always be 3
        int GetChannels();
        void SetChannels(int Channels);

        void WritePixel(int x, int y, GLubyte r, GLubyte g, GLubyte b);

        void UpdateFrame();
        void PollEvents();

        void ClearCanvas();

        bool GetWindowShouldClose();

    // Private functions
    private:
        // Callbacks are fun in c++ classes
        //https://stackoverflow.com/questions/44711290/passing-in-c-method-as-a-function-pointer
        void bind();
        void unbind();
        
        // key callback for GLFW
        void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void KeyCallbackStatic(GLFWwindow* window, int key, int scancode, int action, int mods);

        // Will move to renderer eventually, but for now I need a test case
        void MakeGradient();

    // Private variables
    private:
        bool windowShouldClose;
        GLFWwindow* AppWindow;

        int width = 640;
        int height = 480;
        int channels = 3;
        

        // Actions pertaining to window state are internal, but we want a way to talk to the renderer
        int action = 0;

        std::vector<GLubyte> canvas;

};

