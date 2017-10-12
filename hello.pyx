include "mjmodel.pxd"                                                                                                                                  
include "mjrender.pxd"

cdef mjModel *_model_ptr
cdef mjrContext *_con

def say_hello_to(name):
    print("Hello %s!" % name)

cdef extern from "math.h":
    double sin(double x)

cdef extern from "mujoco.h":
    void mjr_makeContext(const mjModel* m, mjrContext* con, int fontscale);

# if __name__ == '__main__':
say_hello_to('Ethan')

mjr_makeContext(_model_ptr, _con, mjFONTSCALE_150)
