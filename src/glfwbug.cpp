#include "mujoco.h"
#include "glfw3.h"


mjModel* m;
mjrContext c;
char error[300];

int main(int argc, const char** argv) {
  m = mj_loadXML(argv[1], 0, error, 1000);
  glfwInit();
  glfwCreateWindow(800, 800, "Invisible window", NULL, NULL);
  mjr_defaultContext(&c);
  mjr_makeContext(m, &c, mjFONTSCALE_150);
}
