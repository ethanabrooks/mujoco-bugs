//---------------------------------//
//  This file is part of MuJoCo    //
//  Written by Emo Todorov         //
//  Copyright (C) 2017 Roboti LLC  //
//---------------------------------//


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
    // parse numeric arguments
    double duration = 10, fps = 30;

    // initialize OpenGL and MuJoCo
    //initOpenGL();
    if( !glfwInit() )
        mju_error("Could not initialize GLFW");

    // create invisible window, single-buffered
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    GLFWwindow* offscreen = glfwCreateWindow(800, 800, "Invisible window", NULL, NULL);
    if( !offscreen )
        mju_error("Could not create offscreen GLFW window");

    // create visible window, double-buffered
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(800, 800, "Visible window", NULL, NULL);
    if( !window )
        mju_error("Could not create GLFW window");

    // set context to offscreen window
    glfwMakeContextCurrent(window);

    //initMuJoCo(argv[1]);
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

    // get size of active renderbuffer
    mjrRect rect =  mjr_maxViewport(&con);
    int W = rect.width;
    int H = rect.height;

    mjrRect viewport = {0, 0, 0, 0};
    glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

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
    double frametime = 0;
    int framecount = 0;
    while( !glfwWindowShouldClose(window) )
    {
      // update abstract scene
      mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);

      // switch to offscreen context 
      glfwMakeContextCurrent(offscreen);

      // set rendering to offscreen buffer
      mjr_setBuffer(mjFB_OFFSCREEN, &con);
      if( con.currentBuffer!=mjFB_OFFSCREEN )
          printf("Warning: offscreen rendering not supported, using default/window framebuffer\n");

      // render scene in offscreen buffer
      mjr_render(rect, &scn, &con);

      // read rgb and depth buffers
      mjr_readPixels(rgb, depth, rect, &con);

      // write rgb image to file
      fwrite(rgb, 3, W*H, fp);

      // make context current
      glfwMakeContextCurrent(window);

      // set rendering to window buffer
      mjr_setBuffer(mjFB_WINDOW, &con);
      if( con.currentBuffer!=mjFB_WINDOW )
          printf("Warning: unable to select window framebuffer.\n");

      // render scene in window buffer
      mjr_render(viewport, &scn, &con);

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
    mj_deleteData(d);
    mj_deleteModel(m);
    mjr_freeContext(&con);
    mjv_freeScene(&scn);
    mj_deactivate();
    glfwTerminate();

    return 1;
}

