#include "mujoco.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "glfw3.h"

// MuJoCo model and data
mjModel* model = 0;
mjData* data = 0;

// MuJoCo visualization
mjvScene scn;
mjvCamera cam;
mjvOption opt;
mjrContext con;


//-------------------------------- main function ----------------------------------------

int main(int argc, const char** argv)
{

  mj_activate("mjkey.txt");
  char error[1000] = "Could not load xml model";
  model = mj_loadXML("humanoid.xml", NULL, error, 300);
  data = mj_makeData(model);
  mj_resetData(model, data);
  mjv_makeScene(&scn, 1000);
  mjv_defaultCamera(&cam);
  mjv_defaultOption(&opt);
  mjr_defaultContext(&con);
  glfwInit();
  // Creating window glfw
  GLFWwindow* window = glfwCreateWindow(3440, 1440, "mujoco_py", NULL, NULL);
  glfwMakeContextCurrent(window);
  mj_forward(model, data);
  mjr_makeContext(model, &con, mjFONTSCALE_150);
  mjr_setBuffer(mjFB_WINDOW, &con);
  mjv_makeScene(&scn, 1000);
  mjv_defaultCamera(&cam);
  mjv_defaultOption(&opt);
  mjr_defaultContext(&con);
  initOpenGL(0);
  //Found 1 GPUs for rendering. Using device 0.
  //Could not make EGL context current
  mjr_freeContext(&con);
  mjv_freeScene(&scn);
}
