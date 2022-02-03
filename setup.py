# $File: setup.py $
# $LastChangedDate:  $
# $Rev:  $
# Copyright (c) 2014, Gao Wang <gaow@uchicago.edu>
# GNU General Public License (http://www.gnu.org/licenses/gpl.html)

from distutils.core import setup, Extension
# from distutils.dep_util import newer
try:
    from distutils.command.build_py import build_py_2to3 as build_py
except ImportError:
    from distutils.command.build_py import build_py

# monkey-patch for parallel compilation
import multiprocessing, multiprocessing.pool

def compile_parallel(
        self,
        sources,
        output_dir=None,
        macros=None,
        include_dirs=None,
        debug=0,
        extra_preargs=None,
        extra_postargs=None,
        depends=None):

    # Copied from distutils.ccompiler.CCompiler
    macros, objects, extra_postargs, pp_opts, build = self._setup_compile(
        output_dir, macros, include_dirs, sources, depends, extra_postargs)
    cc_args = self._get_cc_args(pp_opts, debug, extra_preargs)
    #
    def _single_compile(obj):

        try:
            src, ext = build[obj]
        except KeyError:
            return
        self._compile(obj, src, ext, cc_args, extra_postargs, pp_opts)
    # convert to list, imap is evaluated on-demand
    list(multiprocessing.pool.ThreadPool(multiprocessing.cpu_count()).imap(_single_compile, objects))
    return objects

import distutils.ccompiler
distutils.ccompiler.CCompiler.compile=compile_parallel

import sys, os, subprocess, platform
from glob import glob
from src import NAME, VERSION
if VERSION is None:
    VERSION = 'rev{}'.format(subprocess.check_output('cat src/.revision', shell=True).strip())

# use ccache to speed up build
try:
    if subprocess.call(['ccache'], stderr = open(os.devnull, "w")):
        os.environ['CC'] = 'ccache clang -Qunused-arguments' if platform.system() == 'Darwin' else 'ccache gcc'
except OSError:
    pass

SWIG_OPTS = ['-c++', '-python', '-O', '-shadow', '-keyword',
             '-w-511', '-w-509', '-outdir', '.']
#SWIG_PYGSL_OPTS = ['-python', '-keyword', '-outdir', '.']


if sys.version_info.major == 2:
    PYVERSION = 'py2'
else:
    SWIG_OPTS.append('-py3')
#    SWIG_PYGSL_OPTS.append('-py3')
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
#WRAPPER_PYGSL_C = getfn('gsl_wrap_{0}.c'.format(PYVERSION), "src")
#WRAPPER_PYGSL_PY = getfn('gsl_{0}.py'.format(PYVERSION), "src")
#WRAPPER_PYGSL_I = getfn('gsl.i', "src")
#WRAPPER_PYBOOSTMATH_CPP = getfn('boostmath_wrap_{0}.cxx'.format(PYVERSION), "src")
#WRAPPER_PYBOOSTMATH_PY = getfn('boostmath_{0}.py'.format(PYVERSION), "src")
#WRAPPER_PYBOOSTMATH_I = getfn('boostmath.i', "src")
#ASSOC_HEADER = getfn(['assoTests.h','assoData.h','action.h','utils.h','lm.h'], "src/assoTests")
#ASSOC_CPP = getfn(['assoData.cpp','action.cpp','utils.cpp','lm.cpp'], "src/assoTests")
#WRAPPER_ASSOC_CPP = getfn('assoTests_wrap_{0}.cxx'.format(PYVERSION), "src")
#WRAPPER_ASSOC_PY = getfn('assoTests_{0}.py'.format(PYVERSION), "src")
#WRAPPER_ASSOC_I = getfn('assoTests.i', "src/assoTests")

# generate wrapper files
try:
    try:
        ret = subprocess.call(['swig', '-python', '-external-runtime', 'src/third/swigpyrun.h'], shell=False)
    except:
        SWIG_SUPPORT = False
        sys.stderr.write('\033[1;91mSWIG program is not avaiable. Using existing wrapper code, which might be problematic.\033[0m\n')
    else:
        SWIG_SUPPORT = True
    #
    if SWIG_SUPPORT and (not os.path.isfile(WRAPPER_PY) or not os.path.isfile(WRAPPER_CPP) or \
        os.path.getmtime(WRAPPER_CPP) < max([os.path.getmtime(x) for x in [WRAPPER_I] + HEADER + CPP])):
        ret = subprocess.call(['swig'] + SWIG_OPTS + ['-o', WRAPPER_CPP, WRAPPER_I], shell=False)
        if ret != 0:
            sys.exit('Failed to generate cstatgen C++ extension.')
        os.rename('cstatgen.py', WRAPPER_PY)
    #
 #   if SWIG_SUPPORT and (not os.path.isfile(WRAPPER_PYGSL_PY) or not os.path.isfile(WRAPPER_PYGSL_C) or \
 #      not os.path.isfile(WRAPPER_PYBOOSTMATH_PY) or not os.path.isfile(WRAPPER_PYBOOSTMATH_CPP)):
 #       ret = subprocess.call(['swig'] + SWIG_PYGSL_OPTS + ['-o', WRAPPER_PYGSL_C, WRAPPER_PYGSL_I], shell=False)
 #       if ret != 0:
 #          sys.exit('Failed to generate gsl extension.')
 #       os.rename('gsl.py', WRAPPER_PYGSL_PY)
 #       #
 #       ret = subprocess.call(['swig'] + SWIG_OPTS + ['-o', WRAPPER_PYBOOSTMATH_CPP, WRAPPER_PYBOOSTMATH_I], shell=False)
 #       if ret != 0:
 #          sys.exit('Failed to generate boost extension.')
 #       os.rename('boostmath.py', WRAPPER_PYBOOSTMATH_PY)
    #
 #   if SWIG_SUPPORT and (not os.path.isfile(WRAPPER_ASSOC_PY) or not os.path.isfile(WRAPPER_ASSOC_CPP) or \
 #     os.path.getmtime(WRAPPER_ASSOC_CPP) < max([os.path.getmtime(x) for x in [WRAPPER_ASSOC_I] + ASSOC_HEADER + ASSOC_CPP])):
 #       ret = subprocess.call(['swig'] + SWIG_OPTS + ['-o', WRAPPER_ASSOC_CPP, WRAPPER_ASSOC_I], shell=False)
 #       if ret != 0:
 #          sys.exit('Failed to generate assoTests extension.')
 #       os.rename('assoTests.py', WRAPPER_ASSOC_PY)
except OSError as e:
    sys.exit('Failed to generate wrapper file: {0}'.format(e))

# Under linux/gcc, lib stdc++ is needed for C++ based extension.
libs = ['stdc++'] if sys.platform == 'linux2' else []
link_args = ["-lm", "-lz"]
#

compile_args_umich = ["-O3", "-shared", "-std=c++11", "-D_FILE_OFFSET_BITS=64", "-D__ZLIB_AVAILABLE__"]#, "-o","umichlib.so","-fPIC"]
                      # "-static", "-static-libgcc", "-static-libstdc++", "-fPIC"]
UMICH_FILES = getfn(["clusters/*.cpp", "libsrc/*.cpp", "merlin/*.cpp", "regression/*.cpp",
                     "rvtests/*.cpp", "base/*.cpp", "pdf/*.cpp", "klib/*.c", "general/*.cpp", "vcf/*.cpp"])
os.system("cd src/third/libMvtnorm; make; cd -")
CSTATGEN_MODULE = [
    Extension('{}._cstatgen'.format(NAME),
              sources = [WRAPPER_CPP] + CPP + UMICH_FILES, # + getfn(LIB_GSL),
              extra_compile_args = compile_args_umich,
    	      extra_link_args = link_args,
              libraries = libs + ["Mvtnorm","gsl"],
              library_dirs = ["src/third/libMvtnorm"],
              include_dirs = getfn([".", "general", "klib", "vcf", "clusters", "libsrc", "base",
                                    "merlin", "regression", "rvtests", "pdf","eigen"]) + \
                                    ["src", "src/third"]
              )
]
#    
#NUM_MODULE = [
#    Extension('{}._gsl'.format(NAME),
#              sources = [WRAPPER_PYGSL_C] + getfn(PY_GSL, 'src/third'),
#              include_dirs = ['src', 'src/third', 'src/third/gsl', 'src/third/gsl/specfunc']
#              ),
#    Extension('{}._boostmath'.format(NAME),
#              sources = [WRAPPER_PYBOOSTMATH_CPP],
#              include_dirs = ['src', 'src/third']
#              )
#]
#ASSOTESTS_MODULE = [
#    Extension('{}._assoTests'.format(NAME),
#              sources = [WRAPPER_ASSOC_CPP] + ASSOC_CPP + getfn(LIB_GSL, 'src/third'),
#              extra_compile_args = ["-O3", "-std=c++11"],
#              libraries = libs,
#              library_dirs = [],
#              include_dirs = ["src/assoTests", "src/third", "src/third/gsl"]
#              )
#]

packages = [NAME]
package_data = {}
#ext_modules = CSTATGEN_MODULE + NUM_MODULE + ASSOTESTS_MODULE
ext_modules = CSTATGEN_MODULE
if sys.version_info.major == 2:
    compile_args_egglib = ["-O3", "-std=c++11", "-UHAVE_LIBBPP_SEQ", "-UHAVE_LIBBPP_CORE", "-UHAVE_LIBGSLCBLAS"]
    # exclude two modules due to lack of gsl and bio++; egglib should have used macro to control for it, though
    EGGLIB_FILES = [x for x in getfn("egglib-cpp/*.cpp", prefix = "src/egglib") if not (x.endswith("ABC.cpp") or x.endswith("BppDiversity.cpp"))]
    EGGLIB_MODULE = [
        Extension('_egglib_binding',
                  sources = ["src/egglib/egglib_binding.cpp"] + EGGLIB_FILES,
                  extra_compile_args = compile_args_egglib,
                  extra_link_args = link_args,
                  libraries = libs,
                  library_dirs = [],
                  include_dirs = getfn(["egglib/egglib-cpp", "egglib"], prefix = "src")
                  )
    ]
    packages += [NAME + ".egglib"]
    package_data[NAME + ".egglib"]=['apps.conf']
    ext_modules += EGGLIB_MODULE

setup(name = NAME,
    version = VERSION,
    description = "Gao Wang's statgen library",
    author = "Gao Wang",
    package_dir = {NAME:'src'},
    cmdclass = {'build_py': build_py},
    packages = packages,
    package_data = package_data,
    ext_modules = ext_modules
     )
