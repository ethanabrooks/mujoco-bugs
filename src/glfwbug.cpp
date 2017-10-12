#include "mujoco.h"
#include "glfw3.h"


mjModel* m;
mjrContext c;
char error[300];

int main(int argc, const char** argv) {
  mjr_defaultContext(&c);
  glfwInit();
  GLFWwindow* window = glfwCreateWindow(800, 800, "Invisible window", NULL, NULL);
  glfwMakeContextCurrent(window);
  m = mj_loadXML(argv[1], 0, error, 1000);
  mjr_makeContext(m, &c, mjFONTSCALE_150);
  mjr_freeContext(&c);
}
