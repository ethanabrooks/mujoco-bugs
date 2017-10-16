#include "mujoco.h"
#include "glfw3.h"
#include <stdio.h>


mjModel* m;
mjData* d;
mjrContext con;
mjrContext offscreen_con;
mjvScene scn;
mjvCamera cam;
mjvOption opt;
mjvPerturb pert;
mjrRect viewport;
mjrRect offscreen;
unsigned char*  rgb_array;

char errstr [300];

int main(int argc, const char** argv) {
  mj_activate("mjkey.txt");

  m = mj_loadXML("/home/ethanbro/.mujoco/mjpro150/model/humanoid.xml", NULL, errstr, 300);
  d = mj_makeData(m);

  mjv_defaultCamera(&cam);
  mjv_defaultOption(&opt);
  mjr_defaultContext(&offscreen_con);
  mjr_defaultContext(&con);

  if( !glfwInit() )
      mju_error("Could not initialize GLFW");
  GLFWwindow* window = glfwCreateWindow(1200, 900, "Demo", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);


  mjr_setBuffer(mjFB_OFFSCREEN, &offscreen_con);
  mjrRect offscreen = mjr_maxViewport(&offscreen_con);
  rgb_array = (unsigned char*)calloc(3 * offscreen.height * offscreen.width,sizeof(*rgb_array));
  if (!rgb_array) {
    printf("Failed to allocate memory for 3 x %d x %d", offscreen.height, offscreen.width);
  }

  //mjr_setBuffer(mjFB_WINDOW, &con);
   
  mjv_makeScene(&scn, 1000);
  mjr_makeContext(m, &offscreen_con, mjFONTSCALE_150);
  mjr_makeContext(m, &con, mjFONTSCALE_150);


  while( !glfwWindowShouldClose(window) ) {
    mj_step(m, d);
    glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

    // sky camera
    cam.type = mjCAMERA_FREE;
    cam.fixedcamid = -1;
    mjv_updateScene(m, d, &opt, &pert, &cam, mjCAT_ALL, &scn);
    mjr_render(viewport, &scn, &con);

    // head camera
    cam.fixedcamid = 0;
    cam.type = mjCAMERA_FIXED;
    mjr_render(offscreen, &scn, &offscreen_con);

    mjr_readPixels(rgb_array, NULL, offscreen, &con); // <-- throws segfault
    //printf("%u\n", rgb_array[200]);
    glfwSwapBuffers(window);
  }

  free(rgb_array);
  glfwTerminate();
  mjv_freeScene(&scn);
  mjr_freeContext(&con);

  // free MuJoCo model and data, deactivate
  mj_deleteData(d);
  mj_deleteModel(m);
  mj_deactivate();

  return 1;
}
