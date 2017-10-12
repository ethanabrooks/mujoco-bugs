#! /usr/bin/env python
from distutils.core import setup
from Cython.Build import cythonize
from os.path import expanduser
from distutils.extension import Extension

extensions = [
    Extension('render',
              sources=['render.pyx', expanduser('src/osmesashim.c')],
              include_dirs=[expanduser('~/.mujoco/mjpro150/include'),
                            expanduser('~/glfw-bug/h')],
              runtime_library_dirs=[expanduser('~/.mujoco/mjpro150/bin')],
              library_dirs=[expanduser('~/.mujoco/mjpro150/bin')],
              libraries=['mujoco150',
                         'glewosmesa', 'OSMesa', 'GL'
                         ],
              extra_compile_args=[
                  '-fopenmp',  # needed for OpenMP
                  '-w'
              ],
              extra_link_args=['-fopenmp'],
              language='c',
              )
]

setup(
    name='Hello world app',
    ext_modules=cythonize(extensions)
)
