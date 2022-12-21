#include "Viewport.h"

#include <filesystem>
#include <stdexcept>
#include <thread>
#include <vector>

#include "CameraMessage.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>

constexpr int cNumChannels = 3;

Viewport::Viewport(int width, int height) :
    canvas(width * height * cNumChannels, 0),
    width(width),
    height(height),
    mIlluminationPercentage(1.0),
    mXAngle(pi / 2),
    mYAngle(0.0),
    mZAngle(0.0),
    mTranslation(point3d(0, 0, 70)),
    mAnimate(false),
    mNumRenderThreads(1)
{
    ClearCanvas();

    // Create the OpenGl context and set up imgui
    if (!glfwInit())
    {
        throw std::runtime_error("glfw failed to init");
    }

    AppWindow = glfwCreateWindow(width, height, "Obj Viewer", NULL, NULL);
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
    std::filesystem::path cwd = std::filesystem::current_path().parent_path() / "TestRender.png";
    std::string cwdString = cwd.string();
    strcpy(mFilePath, cwdString.c_str());

    // Setting up saving default path
    //cwd = std::filesystem::current_path().parent_path() / "teapot-low.obj";
    std::filesystem::path cwdObj = std::filesystem::current_path().parent_path() / "teapot.obj";

    std::string cwdObjString = cwdObj.string();
    strcpy(mFilePathObj, cwdObjString.c_str());
    windowShouldClose = false;

    // Set up imgui
    GuiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(GuiContext);

    // I'm not really sure what ; (void)io does
    // Apparently it's to make the compiler happy because io gets used
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

// Handle cleanup in the object destructor
Viewport::~Viewport()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(GuiContext);

    glfwDestroyWindow(AppWindow);
    glfwTerminate();
}

// There are some keyboard shortcuts in the OpenGl window
void Viewport::PollEvents()
{
        glfwPollEvents();
}

// Handle the OpenGl updates
void Viewport::UpdateFrame()
{
        glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, canvas.data());
        glfwSwapBuffers(AppWindow);
        if (glfwWindowShouldClose(AppWindow))
        {
            windowShouldClose = true;
        }
}

// Poll the gui, all of the controls live in here
CameraMessage Viewport::UpdateGui()
{
    CameraMessage cameraMsg;
    // Number of Rendering Threads
    auto MaxThreads = std::thread::hardware_concurrency() / 2;
    int NumRenderThreads = mNumRenderThreads;

    // Get slider variables from object
    float illuminationSlider = (float) mIlluminationPercentage;

    // I'm mostly using doubles and Dear ImGui uses floats for sliders
    float xAngle = (float) mXAngle;
    float yAngle = (float) mYAngle;
    float zAngle = (float) mZAngle;

    float xTrans = (float) mTranslation.x;
    float yTrans = (float) mTranslation.y;
    float zTrans = (float) mTranslation.z;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    // From the ImGui demo
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    int xPos, yPos;
    glfwGetWindowPos(AppWindow, &xPos, &yPos);
    ImVec2 base_pos(xPos, yPos);
    int xSize, ySize;
    glfwGetWindowSize(AppWindow, &xSize, &ySize);
    ImVec2 base_size(xSize, ySize);

    ImGui::NewFrame();
    {
        // Set up initial position
        ImGui::SetNextWindowPos(ImVec2(base_pos.x + 1.01 * base_size.x,
                                base_pos.y - base_size.y * 0.03),
                                ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(base_size.x, base_size.y / 2 + base_size.y * 0.05), ImGuiCond_Once);
        ImGui::Begin("Test Controls");

        ImGui::SliderInt("Render Threads", &NumRenderThreads, 1, MaxThreads);
        if (NumRenderThreads != mNumRenderThreads)
        {
            ActionReturned = CameraAction::ChangeRenderThreads;
        }

        mNumRenderThreads = NumRenderThreads;
        cameraMsg.mNumRenderThreads = mNumRenderThreads;


        if (ImGui::Button("Render"))
        {
            ActionReturned = CameraAction::DrawWorld;
        }

        if (ImGui::Button("Clear"))
        {
            ActionReturned = CameraAction::ClearCanavas;
        }

        if (ImGui::Button("Gradient"))
        {
            ActionReturned = CameraAction::DrawGradient;
        }

        ImGui::InputText("##asset folder", mFilePathObj, IM_ARRAYSIZE(mFilePathObj));
        ImGui::SameLine();
        if (ImGui::Button("Load Obj File"))
        {
            ActionReturned = CameraAction::LoadObj;
            cameraMsg.mObjFilepath = std::string(mFilePathObj);
            std::cout << "File to load is: " << mFilePathObj << '\n';
        }

        if (ImGui::Button("Rotate World"))
        {
            // degrees to radians the angle slider is in radians for reasons and I already did the conversion in the world transformation
            yAngle += 5 * (pi / 180.0);
            ActionReturned = CameraAction::RotateWorld;
            cameraMsg.mYAngle = yAngle;
            mYAngle = yAngle;
        }

        // Animate rotation
        if (ImGui::Button("Animate Rotation"))
        {
            mAnimate = not mAnimate;
            if (mAnimate)
            {
                mStartTime = std::chrono::system_clock::now();
            }
        }

        if (mAnimate)
        {
            // Adding a delay
            mEndTime = std::chrono::system_clock::now();
            if ((std::chrono::duration_cast<std::chrono::milliseconds> (mEndTime - mStartTime).count() / 1000.0) > 0.5)
            {
                mStartTime = std::chrono::system_clock::now(); 

                // degrees to radians the angle slider is in radians for reasons and I already did the conversion in the world transformation
                yAngle += 5 * (pi / 180.0);
                ActionReturned = CameraAction::RotateWorld;
                cameraMsg.mYAngle = yAngle;
                mYAngle = yAngle;
            }
        }

        ImGui::InputText("## ", mFilePath, IM_ARRAYSIZE(mFilePath));
        ImGui::SameLine();
        if (ImGui::Button("Save"))
        {
            // Do stuff
            std::cout << "File to save is: " << mFilePath << '\n';
            WriteFrame(mFilePath);
        }

        if (ImGui::Button("Close"))
        {
            ActionReturned = CameraAction::StopRender;
            windowShouldClose = true;
        }
        ImGui::End();
    }

    {
        // Set up initial position
        ImGui::SetNextWindowPos(ImVec2(base_pos.x + 1.01 * base_size.x,
            base_pos.y + base_size.y / 2 + base_size.y * 0.2),
            ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(base_size.x, base_size.y / 2), ImGuiCond_Once);

        ImGui::Begin("Object Controls");

        ImGui::SliderFloat("Illumination angle", &illuminationSlider, 0.0, 1.0, "%.1f");
        if (abs(illuminationSlider - mIlluminationPercentage) > 0.05)
        {
            ActionReturned = CameraAction::SliderChanged;
        }

        mIlluminationPercentage = (double)illuminationSlider;
        cameraMsg.mIlluminationPercentage = illuminationSlider;

        // Object rotation angles
        // Comments are to break up the controls
        // X angle
        ImGui::SliderAngle("X angle", &xAngle);

        if (abs(xAngle - mXAngle) > 0.001)
        {
            ActionReturned = CameraAction::SliderChanged;
            mXAngle = (double) xAngle;
        }
        cameraMsg.mXAngle = xAngle * (180.0 / pi);

        // Y angle
        ImGui::SliderAngle("Y angle", &yAngle);

        if (abs(yAngle - mYAngle) > 0.001)
        {
            ActionReturned = CameraAction::SliderChanged;
            mYAngle = (double)yAngle;
        }

        cameraMsg.mYAngle = yAngle * (180.0 / pi);

        // Z angle
        ImGui::SliderAngle("Z angle", &zAngle);

        if (abs(zAngle - mZAngle) > 0.001)
        {
            ActionReturned = CameraAction::SliderChanged;
            mZAngle = (double)zAngle;
        }

        cameraMsg.mZAngle = zAngle * (180.0 / pi);

        // Translation x
        ImGui::SliderFloat("x translation", &xTrans, -10.0f, 10.0f, "%.1f");
        if (abs(xTrans - mTranslation.x) > 0.001)
        {
            ActionReturned = CameraAction::SliderChanged;
        }
        cameraMsg.mTranslation.x = (double) xTrans;

        // Translation y
        ImGui::SliderFloat("y translation", &yTrans, -10.0f, 10.0f, "%.1f");
        if (abs(yTrans - mTranslation.y) > 0.001)
        {
            ActionReturned = CameraAction::SliderChanged;
        }
        cameraMsg.mTranslation.y = yTrans;

        // Translation z
        ImGui::SliderFloat("z translation", &zTrans, 0.0f, 200.0f, "%.1f");
        if (abs(zTrans - mTranslation.z) > 0.001)
        {
            ActionReturned = CameraAction::SliderChanged;
        }
        cameraMsg.mTranslation.z = zTrans;

        // Update member variable
        mTranslation = cameraMsg.mTranslation;

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

    return cameraMsg;

}

// Clear the canvas using the write pixel function, hope that gets inlined
void Viewport::ClearCanvas()
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < (width * cNumChannels); j += cNumChannels)
        {
            for (int channel = 0; channel < cNumChannels; channel++)
            {
                canvas[j + i * width * cNumChannels + channel] = (GLubyte) 0;
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
    int errorValue = stbi_write_png(mFilePath,
        width,
        height,
        cNumChannels,
        (void*)&canvas[width * (height - 1) * cNumChannels],
        -cNumChannels * width);

    return errorValue;
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
        ActionReturned = CameraAction::DrawGradient;
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        ActionReturned = CameraAction::DrawWorld;
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

double Viewport::GetIlluminationPercentage()
{
    return mIlluminationPercentage;
}

CameraAction Viewport::GetGuiAction()
{
    CameraAction temp = ActionReturned;
    ActionReturned = CameraAction::None;

    return temp;
}
