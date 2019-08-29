#!/usr/bin/env python

env = Environment()

from glob import glob

files = glob("*.cpp")

env.Append(CXXFLAGS="-std=c++17")

env.Program('tracer.out', files)

