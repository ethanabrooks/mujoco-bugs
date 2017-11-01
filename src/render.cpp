
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


//-------------------------------- main function ----------------------------------------

int main(int argc, const char** argv)
{
    if( !glfwInit() )
        mju_error("Could not initialize GLFW");

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

    // get size of window
    //int W = window_rect.width;
    //int H = window_rect.height;

    mjrRect viewport =  mjr_maxViewport(&con);
    int W = viewport.width;
    int H = viewport.height;

    // allocate rgb and depth buffers
    unsigned char* rgb = (unsigned char*)malloc(3*W*H);
    if( !rgb )
        mju_error("Could not allocate buffers");

    // create output rgb file
    FILE* fp = fopen("rgb.out", "wb");
    if( !fp )
        mju_error("Could not open rgbfile for writing");

    // main loop
    for( int i = 0; i < 500; i++) {

      mjrRect window_rect = {0, 0, 0, 0};
      glfwGetFramebufferSize(window, &window_rect.width, &window_rect.height);
      cam.fixedcamid = 0;
      cam.type = mjCAMERA_FIXED;
      // write offscreen-rendered pixels to file
      mjr_setBuffer(mjFB_OFFSCREEN, &con);
      if( con.currentBuffer!=mjFB_OFFSCREEN )
          printf("Warning: offscreen rendering not supported, using default/window framebuffer\n");
      mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);
      mjr_render(viewport, &scn, &con);
      mjr_readPixels(rgb, NULL, viewport, &con);
      fwrite(rgb, 3, W*H, fp);

      cam.fixedcamid = -1;
      cam.type = mjCAMERA_FREE;
      mjr_setBuffer(mjFB_WINDOW, &con);
      if( con.currentBuffer!=mjFB_WINDOW )
          printf("Warning: window rendering not supported\n");
      mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);
      mjr_render(window_rect, &scn, &con);

      glfwSwapBuffers(window);
      mj_step(m, d);
    }
    printf("ffmpeg -f rawvideo -pixel_format rgb24 -video_size %dx%d -framerate 60 -i rgb.out -vf 'vflip' video.mp4\n", H, W);

    fclose(fp);
    free(rgb);
    mj_deleteData(d);
    mj_deleteModel(m);
    mjr_freeContext(&con);
    mjv_freeScene(&scn);
    mj_deactivate();
    return 1;
}
