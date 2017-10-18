
#include "mujoco.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "glfw3.h"


//-------------------------------- global data ------------------------------------------

// MuJoCo model and data
mjModel* m = 0;
mjData* d = 0;

// MuJoCo visualization
mjvScene scn;
mjvCamera cam;
mjvOption opt;
mjrContext con;


//-------------------------------- utility functions ------------------------------------



// deallocate everything and deactivate
void closeMuJoCo(void)
{
    mj_deleteData(d);
    mj_deleteModel(m);
    mjr_freeContext(&con);
    mjv_freeScene(&scn);
    mj_deactivate();
}


//-------------------------------- main function ----------------------------------------

int main(int argc, const char** argv)
{
    if( !glfwInit() )
        mju_error("Could not initialize GLFW");

    // create invisible window, single-buffered
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    GLFWwindow* offscreen = glfwCreateWindow(800, 800, "Invisible window", NULL, NULL);
    if( !offscreen )
        mju_error("Could not create GLFW window");

    // create visible window, double-buffered
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(800, 800, "Visible window", NULL, NULL);
    if( !window )
        mju_error("Could not create GLFW window");

    glfwMakeContextCurrent(window);
    mj_activate("mjkey.txt");

    char error[1000] = "Could not load xml model";
    m = mj_loadXML("humanoid.xml", 0, error, 1000);
    if( !m )
        mju_error_s("Load model error: %s", error);
    d = mj_makeData(m);
    mj_forward(m, d);

    // initialize MuJoCo visualization
    mjv_makeScene(&scn, 1000);
    mjv_defaultCamera(&cam);
    mjv_defaultOption(&opt);
    mjr_defaultContext(&con);
    mjr_makeContext(m, &con, 200);

    // select camera
    cam.fixedcamid = 0;
    cam.type = mjCAMERA_FIXED;

    // get size of active renderbuffer
    mjrRect rect =  mjr_maxViewport(&con);
    int W = rect.width;
    int H = rect.height;

    // get size of window
    mjrRect window_rect = {0, 0, 0, 0};
    glfwGetFramebufferSize(window, &window_rect.width, &window_rect.height);

    // allocate rgb and depth buffers
    unsigned char* rgb = (unsigned char*)malloc(3*W*H);
    if( !rgb )
        mju_error("Could not allocate buffers");

    // create output rgb file
    FILE* fp = fopen("rgb.out", "wb");
    if( !fp )
        mju_error("Could not open rgbfile for writing");

    // main loop
    for( int i = 0; i < 100; i++) {
      mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);

      // set rendering to offscreen buffer
      glfwMakeContextCurrent(offscreen);
      mjr_setBuffer(mjFB_OFFSCREEN, &con);
      if( con.currentBuffer!=mjFB_OFFSCREEN )
          printf("Warning: offscreen rendering not supported, using default/window framebuffer\n");

      // write offscreen-rendered pixels to file
      mjr_render(rect, &scn, &con);
      mjr_readPixels(rgb, NULL, rect, &con);
      fwrite(rgb, 3, W*H, fp);

      // render to visible window
      glfwMakeContextCurrent(window);
      mjr_setBuffer(mjFB_WINDOW, &con);
      if( con.currentBuffer!=mjFB_WINDOW )
          printf("Warning: rendering not supported, using default/window framebuffer\n");
      mjr_render(window_rect, &scn, &con);
      glfwSwapBuffers(window);

      // advance simulation
      mj_step(m, d);
    }

    fclose(fp);
    free(rgb);
    closeMuJoCo();
    return 1;
}
