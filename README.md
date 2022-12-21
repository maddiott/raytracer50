# raytracer50
Repository for CS50 Final Project a simple Ray Tracer obj viewer written in C++20

# Dependencies
* GLFW (A cross platform OpenGl wrapper)
  - included in this repo under OutsideLibs  
* Dear imGui (A cross platform imediate mode gui toolkit)
  - included in this repo under OutsideLibs  
* stb_image_writer (used for writting the rendered image to a png)
  - Included under OutsideLibs

# Assets
Mesh of the Utah Teapot

# Program Features
- Basic raytracer (naive intersection of polygons with Lambertian shading)
- The controls are embedded in a simple gui
- Supports reading in a subset of .obj files
- Save the rendered image as file that can be opened in windows photo viewer
- Built using CMake and Visual Studio 2022
