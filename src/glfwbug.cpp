#include <GLFW/glfw3.h>
int main() {
    glfwInit();
    GLFWwindow* handle = glfwCreateWindow(1280, 720, "bug?", NULL, NULL);
    glfwDestroyWindow(handle);
    glfwTerminate();
    return 0;
}
