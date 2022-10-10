#include "Viewport.h"
#include <stdexcept>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

Viewport::Viewport(int width, int height, int channels) : canvas(width * height * channels, 0), width(width), height(height), channels(channels)
{
    // First we'll make a gradient
    MakeGradient();

    // Create the OpenGl context and set up imgui
    if (!glfwInit())
    {
        throw std::runtime_error("glfw failed to init");
    }

    AppWindow = glfwCreateWindow(width, height, "SketchiBoi", NULL, NULL);
    if (!AppWindow)
    {
        glfwTerminate();
        throw std::runtime_error("Window failed to be created");
    }

    glfwMakeContextCurrent(AppWindow);
    
    // Deal with the c/c++ divide
    // https://stackoverflow.com/questions/44711290/passing-in-c-method-as-a-function-pointer
    bind();

    // imgui glfw example does this for vsync, might be a bad idea
    glfwSwapInterval(1);

    windowShouldClose = false;

}

Viewport::~Viewport()
{
    std::cout << "destroy\n";
    glfwDestroyWindow(AppWindow);
    glfwTerminate();
}

void Viewport::PollEvents()
{
        glfwPollEvents();
}

void Viewport::UpdateFrame()
{
        glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, canvas.data());
        glfwSwapBuffers(AppWindow);
        if (glfwWindowShouldClose(AppWindow))
        {
            windowShouldClose = true;
        }
}


// Clear the canvas using the write pixel function, hope that gets inlined
void Viewport::ClearCanvas()
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < (width * channels); j += channels)
        {
            for (int channel = 0; channel < channels; channel++)
            {
                canvas[j + i * width * channels + channel] = (GLubyte) 0;
            }
        }
    }
}

// Sets up a canvas drawing function, technically a bad way to use openGl
// Only supports 3 channels
// https://stackoverflow.com/questions/52694005/drawing-a-pixel-on-opengl-using-gldrawpixels
void Viewport::WritePixel(int x, int y, GLubyte r, GLubyte g, GLubyte b)
{
    int position = 0;

    if ((x < width) && (y < height))
    {
        position = (x + y * width) * 3;
        canvas[position] = r;
        canvas[position + 1] = g;
        canvas [position + 2] = b;
    }
}

void Viewport::MakeGradient()
{
    float r = 0;
    float g = 0;
    float b = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            r = (GLubyte)((((float)i) / ((float)height)) * 255);
            g = (GLubyte)((((float)(i + j)) / ((float)(height + width))) * 255);
            b = (GLubyte)((((float)j) / ((float)width)) * 255);

            canvas[(j + i * width) * 3] = (GLubyte)(255 - r);
            canvas[(j + i * width) * 3 + 1] = (GLubyte)(255 - g);
            canvas[(j + i * width) * 3 + 2] = (GLubyte)(255 - b);
        }
    }

}

// Make GLFW callbacks work with c++ classes
// https://stackoverflow.com/questions/44711290/passing-in-c-method-as-a-function-pointer
void Viewport::bind()
{
    glfwSetWindowUserPointer(AppWindow, this);
    glfwSetKeyCallback(AppWindow, KeyCallbackStatic);
}

void Viewport::KeyCallbackStatic(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Viewport* that = static_cast<Viewport*>(glfwGetWindowUserPointer(window));
    that->KeyCallback(window, key, scancode, action, mods);
}

void Viewport::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        ClearCanvas();
    } 
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        MakeGradient();
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        std::cout << "Spherification is TODO\n";
    }
}

bool Viewport::GetWindowShouldClose()
{
    return windowShouldClose;
}
