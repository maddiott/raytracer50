#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "OutsideLibs/stb_image_writer/stb_image_write.h"

#include <cmath>
#include <filesystem>
#include <iostream>
#include <numbers>
#include <random>
#include <vector>

typedef struct
{
    double x;
    double y;
    double z;
} point3d;

int width = 640;
int height = 480;
int channels = 3;

static int xCoord = width / 2;
static int yCoord = height / 2;

GLubyte* canvas = new GLubyte[width * height * 3];

std::default_random_engine generator;
std::uniform_int_distribution<unsigned int> distribution(0, 255);

void drawSphere(GLubyte* pixelbuf, int width, int height)
{
    // Start with sphere world coord x, y, z
    std::vector<point3d> spheres;
    
    spheres.push_back(point3d(0, 0, 10));
    spheres.push_back(point3d(2, 0, 2));
    spheres.push_back(point3d(0, 2, 4));
    spheres.push_back(point3d(-4, 0, 10));
    double radius = 1;

    // Camera world coord
    std::vector<double> camera = { 0, 0, 1 };

    // Fov in degrees
    double yFov = (std::numbers::pi * 60) / 180;
    double xFov = (std::numbers::pi * 60) / 180;

    // tan(xFov) = xMin / zToCam
    double xMax = tan(xFov / 2);
    double xMin = -xMax;
    double xStep = (xMax - xMin) / width;

    // tan(xFov) = xMin / zToCam
    double yMax = tan(yFov / 2);
    double yMin = -yMax;
    double yStep = (yMax - yMin) / height;

    std::vector<std::vector<point3d>> pixelCoords(height, std::vector<point3d> ( width, point3d(0, 0, 0)));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            pixelCoords[i][j].x = j * xStep + xStep / 2 + xMin;
            pixelCoords[i][j].y = i * yStep + yStep / 2 + yMin;
            pixelCoords[i][j].z = camera[2];
        }
    }


    point3d normVectorPix;
    double magnitude = 0;
    for (point3d center : spheres)
    {
        std::cout << "Center is (" << center.x << ',' << center.y << ',' << center.z << ")\n";
                GLubyte r = (GLubyte) distribution(generator);
                GLubyte g = (GLubyte) distribution(generator);
                GLubyte b = (GLubyte) distribution(generator);

                point3d color = { r, g, b };
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                // Figure out direction (normalize vector)
                magnitude = sqrt((pixelCoords[i][j].x * pixelCoords[i][j].x)
                    + (pixelCoords[i][j].y * pixelCoords[i][j].y)
                    + (pixelCoords[i][j].z * pixelCoords[i][j].z));

                normVectorPix.x = pixelCoords[i][j].x / magnitude;
                normVectorPix.y = pixelCoords[i][j].y / magnitude;
                normVectorPix.z = pixelCoords[i][j].z / magnitude;

                // Send out ray (see Glassner chapter 2, this is the algebraic form of the parametric intersection)
                double A = normVectorPix.x * normVectorPix.x
                    + normVectorPix.y * normVectorPix.y
                    + normVectorPix.z * normVectorPix.z;
                // inefficient since branch, but we'll get rid of this later
                if (abs(A - 1) > 0.0001)
                {
                    std::cout << "might want to take a look at this, magnitude is substantially different than 1 "
                        << A << '\n';
                }

                // Origin is defined as 0 so X_o = Y_o = Z_o = 0
                double B = 2 * ((normVectorPix.x * (-center.x))
                    + (normVectorPix.y * (-center.y))
                    + (normVectorPix.z * (-center.z)));

                double C = (-center.x) * (-center.x)
                    + (-center.y) * (-center.y)
                    + (-center.z) * (-center.z) - (radius * radius);

                // Check intersect
                double discriminant = B * B - 4 * A * C;
                if (discriminant < 0)
                {
                    // If discriminant is less than zero we miss
                    continue;
                }

                double t0 = (-B - sqrt(discriminant)) / 2;
                double t1 = (-B + sqrt(discriminant)) / 2;

                // Closest hit is the one we care about
                double distance = (t0 < t1) ? t0 : t1;

                point3d intersectVector;
                intersectVector.x = distance * normVectorPix.x;
                intersectVector.y = distance * normVectorPix.y;
                intersectVector.z = distance * normVectorPix.z;

                // Check normal
                point3d normal;
                normal.x = (intersectVector.x - center.x) / radius;
                normal.y = (intersectVector.y - center.y) / radius;
                normal.z = (intersectVector.z - center.z) / radius;

                // Draw

                // Find the lamberCosine for shading
                double lambertCosine = normVectorPix.x * normal.x + normVectorPix.y * normal.y + normVectorPix.z * normal.z;
                point3d colorToDraw;

                colorToDraw.x = color.x * abs(lambertCosine);
                colorToDraw.y = color.y * abs(lambertCosine);
                colorToDraw.z = color.z * abs(lambertCosine);

                pixelbuf[(j + i * width) * 3] = (GLubyte)colorToDraw.x;
                pixelbuf[(j + i * width) * 3 + 1] = (GLubyte)colorToDraw.y;
                pixelbuf[(j + i * width) * 3 + 2] = (GLubyte)colorToDraw.z;
            }
        }
    }
}


void moveCursor(int key, int width, int height)
{
    if ((key == GLFW_KEY_LEFT) && ((xCoord - 1) >= 0))
    {
        xCoord--;
    }
    if ((key == GLFW_KEY_RIGHT) && ((xCoord + 1) < width))
    {
        xCoord++;
    }
    if ((key == GLFW_KEY_UP) && ((yCoord + 1) < height))
    {
        yCoord++;
    }
    if ((key == GLFW_KEY_DOWN) && ((yCoord - 1) >= 0))
    {
        yCoord--;
    }
}


void setGradient(GLubyte* pixelbuf, int width, int height)
{
    float r = 0;
    float g = 0;
    float b = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            r = (GLubyte) ((((float) i) / ((float) height)) * 255);
            g = (GLubyte) ((((float) (i + j)) / ((float) (height + width))) * 255);
            b = (GLubyte) ((((float) j) / ((float) width)) * 255);

            pixelbuf[(j + i * width) * 3] = (GLubyte) (255 - r);
            pixelbuf[(j + i * width) * 3 + 1] = (GLubyte) (255 - g);
            pixelbuf[(j + i * width) * 3 + 2] = (GLubyte) (255 - b);
        }
    }
}


void setCanvas (GLubyte r, GLubyte g, GLubyte b, GLubyte* pixelbuf, int width, int height)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < (width * 3); j += 3)
        {
            pixelbuf[j + i * width * 3] = r;
            pixelbuf[j + i * width * 3 + 1] = g;
            pixelbuf[j + i * width * 3 + 2] = b;
        }
    }
}


void clearCanvas(GLubyte* pixelbuf, int width, int height)
{
    setCanvas(0, 0, 0, pixelbuf, width, height);
}

// Sets up a canvas drawing function, technically a bad way to use openGl
// https://stackoverflow.com/questions/52694005/drawing-a-pixel-on-opengl-using-gldrawpixels
void makePixel(int x, int y, GLubyte r, GLubyte g, GLubyte b, GLubyte *pixelbuf, int width, int height)
{
    if ((x < width) && (y < height))
    {
        int position = (x + y * width) * 3;
        pixelbuf[position] = r;
        pixelbuf[position + 1] = g;
        pixelbuf[position + 2] = b;

    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        clearCanvas(canvas, width, height);
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        setGradient(canvas, width, height);
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        drawSphere(canvas, width, height);
    }
    moveCursor(key, width, height);
}

int main()
{
    // Initialize the canvas for OpenGl
    setGradient(canvas, width, height);

    if (!glfwInit())
    {
        std::cerr << "glfw failed to init\n";
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(width, height, "SketchiBoi", NULL, NULL);
    if (!window)
    {
        std::cerr << "window failed\n";
        glfwTerminate();
        return 1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    // imgui glfw example does this for vsync, might be a bad idea
    glfwSwapInterval(1);

    // Setup imgui
    ImGui::CreateContext();

    // I'm not really sure what ; (void)io does
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Never light mode
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/renderer backends
    const char* glsl_version = "#version 130";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // state!
    bool raytrace_enable = true;
    bool window_open = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Setting up saving default path
    std::filesystem::path cwd = std::filesystem::current_path() / "TestRender.png";
    static char FilePath[128];
    std::string cwdString = cwd.string();

    strcpy(FilePath, cwdString.c_str());


    while ((!glfwWindowShouldClose(window)) && window_open)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (((yCoord + xCoord) % 3) == 0)
        {
            makePixel(xCoord, yCoord, 255, 0, 0, canvas, width, height);
        }
        else if (((yCoord + xCoord) % 3) == 1)
        {
            makePixel(xCoord, yCoord, 0, 255, 0, canvas, width, height);
        }
        else if  (((yCoord + xCoord) % 3) == 2)
        {
            makePixel(xCoord, yCoord, 0, 0, 255, canvas, width, height);
        }

        {
            ImGui::Begin("Test Controls");
            ImGui::Text("Renderer Controls");

            if (ImGui::Button("Render"))
            {
                drawSphere(canvas, width, height);
            }

            if (ImGui::Button("Clear"))
            {
                clearCanvas(canvas, width, height);
            }

            if (ImGui::Button("Gradient"))
            {
                setGradient(canvas, width, height);
            }

            ImGui::InputText("", FilePath, IM_ARRAYSIZE(FilePath));
            ImGui::SameLine();
            if (ImGui::Button("Save"))
            {
                // Do stuff
                std::cout << "File to save is: " << FilePath << '\n';

                // OpenGl rendering is flipped, so if we reverse the stride it comes out in the right order
                int error_value = stbi_write_png(FilePath,
                                                 width,
                                                 height,
                                                 channels,
                                                 (void *) &canvas[width * (height - 1) * channels],
                                                 - channels * width);
            }

            if (ImGui::Button("Close"))
            {
                window_open = false;
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, canvas);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

