#include <windows.h>
#include "GLFW\glfw3.h"

#include "CustomWindow.cpp"

using namespace std;

int main(void)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    int sceneWidth = 1200;
    int sceneHeight = 600;
    CustomWindow window = CustomWindow(sceneWidth, sceneHeight, "Polar 2D");

    if (!window.isCreated())
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // loop
    while (!window.shouldClose())
    {
        // render
        window.init(sceneWidth, sceneHeight);

        window.renderScene();

        // swap front and back buffers
        window.swapBuffers();

        // poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    exit(EXIT_SUCCESS);
}
