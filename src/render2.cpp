
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

    // parse numeric arguments
    double duration = 10, fps = 30;

    // initialize OpenGL and MuJoCo
    // init GLFW
    if( !glfwInit() )
        mju_error("Could not initialize GLFW");

    // create invisible window, single-buffered
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    GLFWwindow* offscreen = glfwCreateWindow(800, 800, "Invisible window", NULL, NULL);
    if( !offscreen )
        mju_error("Could not create GLFW window");

    // create invisible window, single-buffered
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(800, 800, "Visible window", NULL, NULL);
    if( !window )
        mju_error("Could not create GLFW window");

    // make context current
    glfwMakeContextCurrent(window);

    // activate
    mj_activate("mjkey.txt");

    // load and compile
    char error[1000] = "Could not load binary model";
    m = mj_loadXML("humanoid.xml", 0, error, 1000);
    if( !m )
        mju_error_s("Load model error: %s", error);

    // make data, run one computation to initialize all fields
    d = mj_makeData(m);
    mj_forward(m, d);

    // initialize MuJoCo visualization
    mjv_makeScene(&scn, 1000);
    mjv_defaultCamera(&cam);
    mjv_defaultOption(&opt);
    mjr_defaultContext(&con);
    mjr_makeContext(m, &con, 200);

    // center and scale view
    //cam.lookat[0] = m->stat.center[0];
    //cam.lookat[1] = m->stat.center[1];
    //cam.lookat[2] = m->stat.center[2];
    //cam.distance = 1.5 * m->stat.extent;
    //cam.fixedcamid = 0;
    //cam.type = mjCAMERA_FIXED;

    // get size of active renderbuffer
    mjrRect rect =  mjr_maxViewport(&con);
    int W = rect.width;
    int H = rect.height;

    mjrRect window_rect = {0, 0, 0, 0};
    glfwGetFramebufferSize(window, &window_rect.width, &window_rect.height);

    // allocate rgb and depth buffers
    unsigned char* rgb = (unsigned char*)malloc(3*W*H);
    float* depth = (float*)malloc(sizeof(float)*W*H);
    if( !rgb || !depth )
        mju_error("Could not allocate buffers");

    // create output rgb file
    FILE* fp = fopen("rgb.out", "wb");
    if( !fp )
        mju_error("Could not open rgbfile for writing");

    // main loop
    for( int i = 0; i < 100; i++) {
    //while (1) {
      mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);

      glfwMakeContextCurrent(offscreen);

      // set rendering to offscreen buffer
      mjr_setBuffer(mjFB_OFFSCREEN, &con);
      if( con.currentBuffer!=mjFB_OFFSCREEN )
          printf("Warning: offscreen rendering not supported, using default/window framebuffer\n");

      /*
      // render scene in offscreen buffer
      mjr_render(rect, &scn, &con);

      // read rgb and depth buffers
      mjr_readPixels(rgb, depth, rect, &con);

      // write rgb image to file
      fwrite(rgb, 3, W*H, fp);
      */

      glfwMakeContextCurrent(window);

      // set rendering to offscreen buffer
      mjr_setBuffer(mjFB_WINDOW, &con);
      if( con.currentBuffer!=mjFB_WINDOW )
          printf("Warning: rendering not supported, using default/window framebuffer\n");

      // render scene in offscreen buffer
      mjr_render(window_rect, &scn, &con);

      // advance simulation
      mj_step(m, d);

      glfwSwapBuffers(window);
    }
    printf("\n");

    // close file, free buffers
    fclose(fp);
    free(rgb);
    free(depth);

    // close MuJoCo and OpenGL
    closeMuJoCo();

    return 1;
}
