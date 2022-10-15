#include "Viewport.h"
#include <filesystem>
#include <stdexcept>


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>

Viewport::Viewport(int width, int height, int channels) :
    canvas(width * height * channels, 0), width(width), height(height), channels(channels) 
{
    ClearCanvas();

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

    // Setting up saving default path
    std::filesystem::path cwd = std::filesystem::current_path() / "TestRender.png";
    std::string cwdString = cwd.string();
    strcpy(FilePath, cwdString.c_str());

    windowShouldClose = false;

    // Set up imgui
    GuiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(GuiContext);

    // I'm not really sure what ; (void)io does
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Never light mode
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/renderer backends
    const char* glsl_version = "#version 130";
    ImGui_ImplGlfw_InitForOpenGL(AppWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


}

Viewport::~Viewport()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(GuiContext);

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

void Viewport::UpdateGui()
{
    //ImGui::SetCurrentContext(GuiContext);
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::Begin("Test Controls");
        ImGui::Text("Renderer Controls");

        if (ImGui::Button("Render"))
        {
            ActionReturned = CameraAction::DrawSphere;
        }

        if (ImGui::Button("Clear"))
        {
            ClearCanvas();
        }

        if (ImGui::Button("Gradient"))
        {
            MakeGradient();
        }

        ImGui::InputText("##", FilePath, IM_ARRAYSIZE(FilePath));
        ImGui::SameLine();
        if (ImGui::Button("Save"))
        {
            // Do stuff
            std::cout << "File to save is: " << FilePath << '\n';
            WriteFrame(FilePath);
        }

        if (ImGui::Button("Close"))
        {
            windowShouldClose = true;
        }
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
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

int Viewport::WriteFrame(const std::string &Filename)
{
    // OpenGl rendering is flipped, so if we reverse the stride it comes out in the right order
    int errorValue = stbi_write_png(FilePath,
        width,
        height,
        channels,
        (void*)&canvas[width * (height - 1) * channels],
        -channels * width);

    return errorValue;
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
        ActionReturned = CameraAction::DrawSphere;
    }
}

bool Viewport::GetWindowShouldClose()
{
    return windowShouldClose;
}

int Viewport::GetHeight()
{
    return height;
}

int Viewport::GetWidth()
{
    return width;
}

CameraAction Viewport::GetGuiAction()
{
    CameraAction temp = ActionReturned;
    ActionReturned = CameraAction::None;

    return temp;
}
