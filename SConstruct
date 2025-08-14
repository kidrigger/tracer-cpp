#!/usr/bin/env python

env = Environment()

from glob import glob

env.sources = []

env.Append(CXXFLAGS=['-std=c++20', '-g', '-Wall'])
env.Append(CPPPATH=['#', '#threadpool'])

libraries = ["pthread"]

def add_sources(self, files):
    from glob import glob
    files = glob(files)
    for file_ in files:
        self.sources.append(env.Object(file_))


env.add_sources = add_sources

env.add_sources(env, '*.cpp')

Export('env')

SConscript('#material/SCsub')
SConscript('#geometry/SCsub')
SConscript('#textures/SCsub')
SConscript('#core/SCsub')
SConscript('#thirdparty/SCsub')

env.Program('#bin/tracer.out', env.sources, LIBS=libraries)
