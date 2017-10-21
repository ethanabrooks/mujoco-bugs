
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
    mj_activate("mjkey.txt");

    char error[1000] = "Could not load xml model";
    m = mj_loadXML("humanoid.xml", 0, error, 1000);
    if( !m )
        mju_error_s("Load model error: %s", error);
    d = mj_makeData(m);

    // initialize MuJoCo visualization
    mjv_makeScene(&scn, 1000);
    mjv_defaultCamera(&cam);
    mjv_defaultOption(&opt);
    mjr_defaultContext(&con);

    if( !glfwInit() )
        mju_error("Could not initialize GLFW");

    // create visible window, double-buffered
    GLFWwindow* window = glfwCreateWindow(3440, 1440, "Visible window", NULL, NULL);
    if( !window )
        mju_error("Could not create GLFW window");
    glfwMakeContextCurrent(window);
    glfwMakeContextCurrent(window);
    mj_forward(m, d);
    mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);

    mjr_makeContext(m, &con, 200);

    // get size of window
    mjrRect viewport = {0, 0, 0, 0};
    glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

    mjrRect viewport2 = {0, 0, 0, 0};
    viewport2.width = 400;
    viewport2.height = 400;
    int W = viewport2.width;
    int H = viewport2.height;

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
      cam.fixedcamid = -1;
      cam.type = mjCAMERA_FREE;
      mjv_updateCamera(m, d, &cam, &scn);
      mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);
      //glfwMakeContextCurrent(window);

      // write offscreen-rendered pixels to file
      mjr_setBuffer(mjFB_OFFSCREEN, &con);
      if (con.currentBuffer!=mjFB_OFFSCREEN)
        printf("Warning: offscreen rendering not supported, using default/window framebuffer\n");
      mjr_render(viewport2, &scn, &con);
      // read_pixels
      mjr_readPixels(rgb, NULL, viewport2, &con);
      fwrite(rgb, 3, W*H, fp);

      cam.fixedcamid = 0;
      cam.type = mjCAMERA_FIXED;
      mjv_updateCamera(m, d, &cam, &scn);
      //mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);
      mjr_setBuffer(mjFB_WINDOW, &con);
      if (con.currentBuffer!=mjFB_WINDOW)
        printf("Warning: unable to render in window.\n"); 
      mjr_render(viewport, &scn, &con);

      glfwSwapBuffers(window);
      mj_step(m, d);
    }
    printf("ffmpeg -y -f rawvideo -pixel_format rgb24 -video_size %dx%d -framerate 60 -i rgb.out -vf \"vflip\" video.mp4\n", W, H);

    fclose(fp);
    free(rgb);
    mj_deleteData(d);
    mj_deleteModel(m);
    mjr_freeContext(&con);
    mjv_freeScene(&scn);
    mj_deactivate();
    return 0;
}
