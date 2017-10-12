include "mjmodel.pxd"
include "mjrender.pxd"
include "mujoco.pxd"

import glfw
from libc.stdlib cimport malloc, free

cdef mjModel *m
cdef mjrContext c
cdef char errstr[300]

cdef extern from "mujoco.h":
    void mjr_makeContext(const mjModel*, mjrContext*, int fontscale);

path = 'models/navigate.xml'
m = mj_loadXML(path.encode(), NULL, errstr, 300)
glfw.init()
window = glfw.create_window(10, 10, "render", None, None)
mjr_defaultContext(&c)
mjr_makeContext(m, &c, mjFONTSCALE_150)
