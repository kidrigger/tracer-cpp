#!/usr/bin/env python

env = Environment()

from glob import glob

files = glob("*.cpp")

env.Append(CXXFLAGS=["-std=c++17", "-g"])
env.Append(CPPPATH="threadpool")

libraries = ["pthread"]

env.Program('tracer.out', files, LIBS=libraries)

