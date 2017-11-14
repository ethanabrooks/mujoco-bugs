
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

GLFWwindow* initGlfw() {
    if( !glfwInit() )
        mju_error("Could not initialize GLFW");

    // create visible window, double-buffered
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(800, 800, "Visible window", NULL, NULL);
    if( !window )
        mju_error("Could not create GLFW window");

    glfwMakeContextCurrent(window);
    return window;
}

int initMujoco(char const* filepath) {
    mj_activate("mjkey.txt");

    char error[1000] = "Could not load xml model";
    m = mj_loadXML(filepath, 0, error, 1000);
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
    return 0;
}

int renderOffscreen(unsigned char* rgb, int height, int width) {
      mjrRect viewport = {0, 0, height, width};
      cam.fixedcamid = 0;
      cam.type = mjCAMERA_FIXED;
      // write offscreen-rendered pixels to file
      mjr_setBuffer(mjFB_OFFSCREEN, &con);
      if( con.currentBuffer!=mjFB_OFFSCREEN )
          printf("Warning: offscreen rendering not supported, using default/window framebuffer\n");
      mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);
      mjr_render(viewport, &scn, &con);
      mjr_readPixels(rgb, NULL, viewport, &con);
      return 0;
}

int renderOnscreen(GLFWwindow* window) {
      mjrRect window_rect = {0, 0, 0, 0};
      glfwGetFramebufferSize(window, &window_rect.width, &window_rect.height);
      cam.fixedcamid = -1;
      cam.type = mjCAMERA_FREE;
      mjr_setBuffer(mjFB_WINDOW, &con);
      if( con.currentBuffer!=mjFB_WINDOW )
          printf("Warning: window rendering not supported\n");
      mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);
      mjr_render(window_rect, &scn, &con);
      glfwSwapBuffers(window);
      return 0;
}

int close(FILE* fp, unsigned char* rgb) {
    fclose(fp);
    free(rgb);
    mj_deleteData(d);
    mj_deleteModel(m);
    mjr_freeContext(&con);
    mjv_freeScene(&scn);
    mj_deactivate();
}


//-------------------------------- main function ----------------------------------------

int main(int argc, const char** argv)
{
    int H = 800;
    int W = 800;
    char const* filepath = "humanoid.xml";

    GLFWwindow* window = initGlfw();
    initMujoco(filepath);

    // allocate rgb and depth buffers
    unsigned char* rgb = (unsigned char*)malloc(3*H*W);
    if( !rgb )
        mju_error("Could not allocate buffers");

    // create output rgb file
    FILE* fp = fopen("rgb.out", "wb");
    if( !fp )
        mju_error("Could not open rgbfile for writing");

    // main loop
    for( int i = 0; i < 500; i++) {
      renderOffscreen(rgb, H, W);
      fwrite(rgb, 3, H * W, fp);
      renderOnscreen(window);
      mj_step(m, d);
    }
    printf("ffmpeg -f rawvideo -pixel_format rgb24 -video_size %dx%d -framerate 60 -i rgb.out -vf 'vflip' video.mp4\n", H, W);

    return 0;
}
