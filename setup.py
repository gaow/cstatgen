# $File: setup.py $
# $LastChangedDate:  $
# $Rev:  $
# Copyright (c) 2014, Gao Wang <ewanggao@gmail.com>
# GNU General Public License (http://www.gnu.org/licenses/gpl.html)

from distutils.core import setup, Extension
try:
   from distutils.command.build_py import build_py_2to3 as build_py
except ImportError:
   from distutils.command.build_py import build_py

import sys, os, subprocess
from glob import glob
from itertools import chain
from src import NAME, VERSION

if sys.platform == "win32":
    sys.exit('Windows OS is not supported.')
    
SWIG_OPTS = ['-c++', '-python', '-O', '-shadow', '-keyword',
             '-w-511', '-w-509', '-outdir', '.']

if sys.version_info.major == 2:
    PYVERSION = 'py2'
else:
    SWIG_OPTS.append('-py3')
    PYVERSION = 'py3'
#
def getfn(fn, prefix = "src/umich"):
    if type(fn) is list:
        return sum([glob(os.path.join(prefix, x)) if "*" in x else [os.path.join(prefix, x)] for x in fn], [])
    else:
        return glob(os.path.join(prefix, fn)) if "*" in fn else os.path.join(prefix, fn)
#
HEADER = getfn("*.hpp", "src")
CPP = getfn("*.cpp", "src")
WRAPPER_CPP = getfn('cstatgen_{0}.cxx'.format(PYVERSION), "src")
WRAPPER_PY = getfn('cstatgen_{0}.py'.format(PYVERSION), "src")
WRAPPER_I = getfn('cstatgen.i', "src")

# generate wrapper files
try:
    ret = subprocess.call(['swig', '-python', '-external-runtime', 'swigpyrun.h'], shell=False)
    if ret != 0:
        sys.exit('Failed to generate swig runtime header file. Is "swig" installed?')
    #
    if (not os.path.isfile(WRAPPER_PY) or not os.path.isfile(WRAPPER_CPP) or \
        os.path.getmtime(WRAPPER_CPP) < max([os.path.getmtime(x) for x in [WRAPPER_I] + HEADER + CPP])):
        ret = subprocess.call(['swig'] + SWIG_OPTS + ['-o', WRAPPER_CPP, WRAPPER_I], shell=False)
        if ret != 0:
            sys.exit('Failed to generate C++ extension.')
        os.rename('cstatgen.py', WRAPPER_PY)
    os.remove('swigpyrun.h')
except OSError as e:
    sys.exit('Failed to generate wrapper file: {0}'.format(e))
#
UMICH_FILES = getfn(["clusters/*.cpp", "libsrc/*.cpp", "merlin/*.cpp",
                     "pdf/*.cpp", "klib/*.c", "general/*.cpp", "vcf/*.cpp"])
# Under linux/gcc, lib stdc++ is needed for C++ based extension.
libs = ['stdc++'] if sys.platform == 'linux2' else []
compile_args = ["-O3", "-march=native", "-std=c++11", "-D_FILE_OFFSET_BITS=64", "-D__ZLIB_AVAILABLE__"]
link_args = ["-lm", "-lz"]
#
CSTATGEN_MODULE = [
    Extension('{}._cstatgen'.format(NAME),
              sources = [WRAPPER_CPP] + CPP + UMICH_FILES,
              extra_compile_args = compile_args,
    	      extra_link_args = link_args,
              libraries = libs,
              library_dirs = [],
              include_dirs = getfn(["general", "klib", "vcf", "clusters", "libsrc", "merlin", "pdf"]) + ["src"]
              )
]
setup(name = NAME,
    version = VERSION,
    description = "Gao Wang's statgen library",
    author = "Gao Wang",
    packages = [NAME],
    package_dir = {NAME:'src'},
    cmdclass = {'build_py': build_py},
    ext_modules = CSTATGEN_MODULE
)
