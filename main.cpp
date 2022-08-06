#include <GLFW/glfw3.h>
#include <iostream>


int width = 640;
int height = 480;

static int xCoord = width / 2;
static int yCoord = height / 2;

GLubyte* canvas = new GLubyte[width * height * 3];


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

            pixelbuf[(j + i * width) * 3] = (GLubyte) b;
            pixelbuf[(j + i * width) * 3 + 1] = (GLubyte) r;
            pixelbuf[(j + i * width) * 3 + 2] = (GLubyte) g;
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

void display(GLFWwindow* window)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, canvas);
    glfwSwapBuffers(window);
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_LEFT)
    {
        xCoord--;
    }
    if (key == GLFW_KEY_RIGHT)
    {
        xCoord++;
    }
    if (key == GLFW_KEY_UP)
    {
        yCoord++;
    }
    if (key == GLFW_KEY_DOWN)
    {
        yCoord--;
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        clearCanvas(canvas, width, height);
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        setGradient(canvas, width, height);
    }
}

int main()
{
    setGradient(canvas, width, height);

    if (!glfwInit())
    {
        std::cerr << "glfw failed to init\n";
    }

    GLFWwindow* window = glfwCreateWindow(width, height, "SketchiBoi", NULL, NULL);
    if (!window)
    {
        std::cerr << "window failed\n";
        glfwTerminate();
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwPollEvents();
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
       
        glClear(GL_COLOR_BUFFER_BIT);
        display(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

