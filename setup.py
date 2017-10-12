#! /usr/bin/env python
from distutils.core import setup
from Cython.Build import cythonize
from os.path import expanduser
from distutils.extension import Extension
import numpy

extensions = [
    Extension('hello', ['hello.pyx'],
              include_dirs=[expanduser('~/.mujoco/mjpro150/include')],
              library_dirs=[expanduser('~/.mujoco/mjpro150/bin')],
              libraries=['mujoco150', 'glew', 'gl', ],
              )
]

setup(
    name='Hello world app',
    ext_modules=cythonize(extensions)
)
