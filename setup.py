# $File: setup.py $
# $LastChangedDate:  $
# $Rev:  $
# Copyright (c) 2014, Gao Wang <ewanggao@gmail.com>
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

if sys.platform != "linux2":
    sys.exit('{} platform is not supported.'.format(sys.platform))


# use ccache to speed up build
try:
    if subprocess.call(['ccache'], stderr = open(os.devnull, "w")):
        os.environ['CC'] = 'ccache clang -Qunused-arguments' if platform.system() == 'Darwin' else 'ccache gcc'
except OSError:
    pass
    
SWIG_OPTS = ['-c++', '-python', '-O', '-shadow', '-keyword',
             '-w-511', '-w-509', '-outdir', '.']
SWIG_PYGSL_OPTS = ['-python', '-keyword', '-outdir', '.']


if sys.version_info.major == 2:
    PYVERSION = 'py2'
else:
    SWIG_OPTS.append('-py3')
    SWIG_PYGSL_OPTS.append('-py3')
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
WRAPPER_PYGSL_C = getfn('gsl_wrap_{0}.c'.format(PYVERSION), "src")
WRAPPER_PYGSL_PY = getfn('gsl_{0}.py'.format(PYVERSION), "src")
WRAPPER_PYGSL_I = getfn('gsl.i', "src")
WRAPPER_PYBOOSTMATH_CPP = getfn('boostmath_wrap_{0}.cxx'.format(PYVERSION), "src")
WRAPPER_PYBOOSTMATH_PY = getfn('boostmath_{0}.py'.format(PYVERSION), "src")
WRAPPER_PYBOOSTMATH_I = getfn('boostmath.i', "src")
ASSOC_HEADER = getfn(['assoTests.h','assoData.h','action.h','utils.h','lm.h'], "src/assoTests")
ASSOC_CPP = getfn(['assoData.cpp','action.cpp','utils.cpp','lm.cpp'], "src/assoTests")
WRAPPER_ASSOC_CPP = getfn('assoTests_wrap_{0}.cxx'.format(PYVERSION), "src")
WRAPPER_ASSOC_PY = getfn('assoTests_{0}.py'.format(PYVERSION), "src")
WRAPPER_ASSOC_I = getfn('assoTests.i', "src/assoTests")

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
            sys.exit('Failed to generate cstatgen C++ extension.')
        os.rename('cstatgen.py', WRAPPER_PY)
    #
    if (not os.path.isfile(WRAPPER_PYGSL_PY) or not os.path.isfile(WRAPPER_PYGSL_C) or \
       not os.path.isfile(WRAPPER_PYBOOSTMATH_PY) or not os.path.isfile(WRAPPER_PYBOOSTMATH_CPP)):
        ret = subprocess.call(['swig'] + SWIG_PYGSL_OPTS + ['-o', WRAPPER_PYGSL_C, WRAPPER_PYGSL_I], shell=False)
        if ret != 0:
           sys.exit('Failed to generate gsl extension.')
        os.rename('gsl.py', WRAPPER_PYGSL_PY)
        #
        ret = subprocess.call(['swig'] + SWIG_OPTS + ['-o', WRAPPER_PYBOOSTMATH_CPP, WRAPPER_PYBOOSTMATH_I], shell=False)
        if ret != 0:
           sys.exit('Failed to generate boost extension.')
        os.rename('boostmath.py', WRAPPER_PYBOOSTMATH_PY)
    #
    if (not os.path.isfile(WRAPPER_ASSOC_PY) or not os.path.isfile(WRAPPER_ASSOC_CPP) or \
      os.path.getmtime(WRAPPER_ASSOC_CPP) < max([os.path.getmtime(x) for x in ASSOC_HEADER + ASSOC_CPP])):
        ret = subprocess.call(['swig'] + SWIG_OPTS + ['-o', WRAPPER_ASSOC_CPP, WRAPPER_ASSOC_I], shell=False)
        if ret != 0:
           sys.exit('Failed to generate assoTests extension.')
        os.rename('assoTests.py', WRAPPER_ASSOC_PY)
    os.rename('swigpyrun.h', 'src/assoTests/swigpyrun.h')
except OSError as e:
    sys.exit('Failed to generate wrapper file: {0}'.format(e))

# Under linux/gcc, lib stdc++ is needed for C++ based extension.
libs = ['stdc++'] if sys.platform == 'linux2' else []
link_args = ["-lm", "-lz"]
#
LIB_GSL = [
   'gsl/error.c',
   'gsl/sys/infnan.c',
   'gsl/sys/coerce.c',
   'gsl/sys/fdiv.c',
   'gsl/sys/pow_int.c',
   'gsl/sys/fcmp.c',
   'gsl/sys/log1p.c',
   'gsl/sys/invhyp.c',
   'gsl/complex/math.c',
   'gsl/specfunc/beta.c',
   'gsl/specfunc/psi.c',
   'gsl/specfunc/trig.c',
   'gsl/specfunc/exp.c',
   'gsl/specfunc/expint.c',
   'gsl/specfunc/log.c',
   'gsl/specfunc/erfc.c',
   'gsl/specfunc/zeta.c',
   'gsl/specfunc/elementary.c',
   'gsl/specfunc/gamma.c',
   'gsl/specfunc/gamma_inc.c',
   'gsl/rng/rng.c',
   'gsl/rng/default.c',
   'gsl/rng/mt.c',
   'gsl/rng/types.c',
   'gsl/randist/binomial.c',
   'gsl/randist/binomial_tpe.c',
   'gsl/randist/beta.c',
   'gsl/randist/exponential.c',
   'gsl/randist/geometric.c',
   'gsl/randist/nbinomial.c',
   'gsl/randist/poisson.c',
   'gsl/randist/multinomial.c',
   'gsl/randist/chisq.c',
   'gsl/randist/gauss.c',
   'gsl/randist/tdist.c',
   'gsl/randist/gausszig.c',
   'gsl/randist/gamma.c',
   'gsl/randist/hyperg.c',
   'gsl/cdf/binomial.c',
   'gsl/cdf/beta.c',
   'gsl/cdf/betainv.c',
   'gsl/cdf/gauss.c',
   'gsl/cdf/gaussinv.c',
   'gsl/cdf/tdist.c',
   'gsl/cdf/tdistinv.c',
   'gsl/cdf/chisq.c',
   'gsl/cdf/chisqinv.c',
   'gsl/cdf/gamma.c',
   'gsl/cdf/gammainv.c',
   'gsl/cdf/hypergeometric.c',
   'gsl/cdf/poisson.c',
   #
   'gsl/blas/blas.c',
   'gsl/cblas/caxpy.c',
   'gsl/cblas/ccopy.c',
   'gsl/cblas/cdotc_sub.c',
   'gsl/cblas/cdotu_sub.c',
   'gsl/cblas/cgbmv.c',
   'gsl/cblas/cgemm.c',
   'gsl/cblas/cgemv.c',
   'gsl/cblas/cgerc.c',
   'gsl/cblas/cgeru.c',
   'gsl/cblas/chbmv.c',
   'gsl/cblas/chemm.c',
   'gsl/cblas/chemv.c',
   'gsl/cblas/cher2.c',
   'gsl/cblas/cher2k.c',
   'gsl/cblas/cher.c',
   'gsl/cblas/cherk.c',
   'gsl/cblas/chpmv.c',
   'gsl/cblas/chpr2.c',
   'gsl/cblas/chpr.c',
   'gsl/cblas/cscal.c',
   'gsl/cblas/csscal.c',
   'gsl/cblas/cswap.c',
   'gsl/cblas/csymm.c',
   'gsl/cblas/csyr2k.c',
   'gsl/cblas/csyrk.c',
   'gsl/cblas/ctbmv.c',
   'gsl/cblas/ctbsv.c',
   'gsl/cblas/ctpmv.c',
   'gsl/cblas/ctpsv.c',
   'gsl/cblas/ctrmm.c',
   'gsl/cblas/ctrmv.c',
   'gsl/cblas/ctrsm.c',
   'gsl/cblas/ctrsv.c',
   'gsl/cblas/dasum.c',
   'gsl/cblas/daxpy.c',
   'gsl/cblas/dcopy.c',
   'gsl/cblas/ddot.c',
   'gsl/cblas/dgbmv.c',
   'gsl/cblas/dgemm.c',
   'gsl/cblas/dgemv.c',
   'gsl/cblas/dger.c',
   'gsl/cblas/dnrm2.c',
   'gsl/cblas/drot.c',
   'gsl/cblas/drotg.c',
   'gsl/cblas/drotm.c',
   'gsl/cblas/drotmg.c',
   'gsl/cblas/dsbmv.c',
   'gsl/cblas/dscal.c',
   'gsl/cblas/dsdot.c',
   'gsl/cblas/dspmv.c',
   'gsl/cblas/dspr2.c',
   'gsl/cblas/dspr.c',
   'gsl/cblas/dswap.c',
   'gsl/cblas/dsymm.c',
   'gsl/cblas/dsymv.c',
   'gsl/cblas/dsyr2.c',
   'gsl/cblas/dsyr2k.c',
   'gsl/cblas/dsyr.c',
   'gsl/cblas/dsyrk.c',
   'gsl/cblas/dtbmv.c',
   'gsl/cblas/dtbsv.c',
   'gsl/cblas/dtpmv.c',
   'gsl/cblas/dtpsv.c',
   'gsl/cblas/dtrmm.c',
   'gsl/cblas/dtrmv.c',
   'gsl/cblas/dtrsm.c',
   'gsl/cblas/dtrsv.c',
   'gsl/cblas/dzasum.c',
   'gsl/cblas/dznrm2.c',
   'gsl/cblas/hypot.c',
   'gsl/cblas/icamax.c',
   'gsl/cblas/idamax.c',
   'gsl/cblas/isamax.c',
   'gsl/cblas/izamax.c',
   'gsl/cblas/sasum.c',
   'gsl/cblas/saxpy.c',
   'gsl/cblas/scasum.c',
   'gsl/cblas/scnrm2.c',
   'gsl/cblas/scopy.c',
   'gsl/cblas/sdot.c',
   'gsl/cblas/sdsdot.c',
   'gsl/cblas/sgbmv.c',
   'gsl/cblas/sgemm.c',
   'gsl/cblas/sgemv.c',
   'gsl/cblas/sger.c',
   'gsl/cblas/snrm2.c',
   'gsl/cblas/srot.c',
   'gsl/cblas/srotg.c',
   'gsl/cblas/srotm.c',
   'gsl/cblas/srotmg.c',
   'gsl/cblas/ssbmv.c',
   'gsl/cblas/sscal.c',
   'gsl/cblas/sspmv.c',
   'gsl/cblas/sspr2.c',
   'gsl/cblas/sspr.c',
   'gsl/cblas/sswap.c',
   'gsl/cblas/ssymm.c',
   'gsl/cblas/ssymv.c',
   'gsl/cblas/ssyr2.c',
   'gsl/cblas/ssyr2k.c',
   'gsl/cblas/ssyr.c',
   'gsl/cblas/ssyrk.c',
   'gsl/cblas/stbmv.c',
   'gsl/cblas/stbsv.c',
   'gsl/cblas/stpmv.c',
   'gsl/cblas/stpsv.c',
   'gsl/cblas/strmm.c',
   'gsl/cblas/strmv.c',
   'gsl/cblas/strsm.c',
   'gsl/cblas/strsv.c',
   'gsl/cblas/xerbla.c',
   'gsl/cblas/zaxpy.c',
   'gsl/cblas/zcopy.c',
   'gsl/cblas/zdotc_sub.c',
   'gsl/cblas/zdotu_sub.c',
   'gsl/cblas/zdscal.c',
   'gsl/cblas/zgbmv.c',
   'gsl/cblas/zgemm.c',
   'gsl/cblas/zgemv.c',
   'gsl/cblas/zgerc.c',
   'gsl/cblas/zgeru.c',
   'gsl/cblas/zhbmv.c',
   'gsl/cblas/zhemm.c',
   'gsl/cblas/zhemv.c',
   'gsl/cblas/zher2.c',
   'gsl/cblas/zher2k.c',
   'gsl/cblas/zher.c',
   'gsl/cblas/zherk.c',
   'gsl/cblas/zhpmv.c',
   'gsl/cblas/zhpr2.c',
   'gsl/cblas/zhpr.c',
   'gsl/cblas/zscal.c',
   'gsl/cblas/zswap.c',
   'gsl/cblas/zsymm.c',
   'gsl/cblas/zsyr2k.c',
   'gsl/cblas/zsyrk.c',
   'gsl/cblas/ztbmv.c',
   'gsl/cblas/ztbsv.c',
   'gsl/cblas/ztpmv.c',
   'gsl/cblas/ztpsv.c',
   'gsl/cblas/ztrmm.c',
   'gsl/cblas/ztrmv.c',
   'gsl/cblas/ztrsm.c',
   'gsl/cblas/ztrsv.c',
   #
   'gsl/linalg/svd.c',
   'gsl/linalg/lu.c',
   'gsl/linalg/bidiag.c',
   'gsl/linalg/householder.c',
   'gsl/matrix/matrix.c',
   'gsl/matrix/submatrix.c',
   'gsl/matrix/rowcol.c',
   'gsl/matrix/getset.c',
   'gsl/matrix/init.c',
   'gsl/vector/init.c',
   'gsl/matrix/swap.c',
   'gsl/vector/vector.c',
   'gsl/vector/copy.c',
   'gsl/vector/swap.c',
   'gsl/vector/subvector.c',
   'gsl/vector/oper.c',
   'gsl/matrix/oper.c',
   'gsl/matrix/copy.c',
   'gsl/block/init.c',
   #
   'gsl/permutation/init.c',
   'gsl/permutation/inline.c',
   'gsl/permutation/permutation.c',
   'gsl/permutation/permute.c',
   #
   'gsl/min/brent.c',
   'gsl/min/fsolver.c',
   'gsl/min/convergence.c'
    ]


PY_GSL = [
    'gsl/error.c', 
    'gsl/sys/infnan.c',
    'gsl/sys/coerce.c',
    'gsl/sys/fdiv.c',
    'gsl/sys/pow_int.c',
    'gsl/sys/fcmp.c',
    'gsl/sys/log1p.c',
    'gsl/sys/invhyp.c',
    'gsl/sys/expm1.c',
    'gsl/complex/math.c',
    'gsl/specfunc/beta.c',
    'gsl/specfunc/elementary.c',
    'gsl/specfunc/erfc.c',
    'gsl/specfunc/exp.c',
    'gsl/specfunc/expint.c',
    'gsl/specfunc/log.c',
    'gsl/specfunc/psi.c',
    'gsl/specfunc/gamma.c',
    'gsl/specfunc/gamma_inc.c',
    'gsl/specfunc/trig.c',
    'gsl/specfunc/zeta.c',
    'gsl/cdf/beta.c',
    'gsl/cdf/betainv.c',
    'gsl/cdf/binomial.c',
    'gsl/cdf/gauss.c',
    'gsl/cdf/gaussinv.c',
    'gsl/cdf/tdist.c',
    'gsl/cdf/tdistinv.c',
    'gsl/cdf/exponential.c',
    'gsl/cdf/exponentialinv.c',
    # pdf functions for gamma, binomial, t, poisson and beta distributions are copied to gsl.i
    # to avoid subsequent inclusion of RNG related functions.
    #'gsl/randist/gamma.c',
    #'gsl/randist/tdist.c',
    #'gsl/randist/binomial.c',
    #'gsl/randist/beta.c',
    #'gsl/randist/poisson.c',
    'gsl/cdf/gamma.c',
    'gsl/cdf/gammainv.c',
    'gsl/cdf/chisq.c',
    'gsl/cdf/chisqinv.c',
    'gsl/cdf/poisson.c',
]

compile_args_umich = ["-O3", "-shared", "-std=c++11", "-D_FILE_OFFSET_BITS=64", "-D__ZLIB_AVAILABLE__"]
                      # "-static", "-static-libgcc", "-static-libstdc++", "-fPIC"]
UMICH_FILES = getfn(["clusters/*.cpp", "libsrc/*.cpp", "merlin/*.cpp", "regression/*.cpp",
                     "rvtests/*.cpp", "base/*.cpp", "pdf/*.cpp", "klib/*.c", "general/*.cpp", "vcf/*.cpp"])
os.system("cd src/third/libMvtnorm; make; cd -")
CSTATGEN_MODULE = [
    Extension('{}._cstatgen'.format(NAME),
              sources = [WRAPPER_CPP] + CPP + UMICH_FILES + getfn(LIB_GSL),
              extra_compile_args = compile_args_umich,
    	      extra_link_args = link_args,
              libraries = libs + ["Mvtnorm"],
              library_dirs = ["src/third/libMvtnorm"],
              include_dirs = getfn([".", "general", "klib", "vcf", "clusters", "libsrc", "base",
                                    "merlin", "regression", "rvtests", "pdf", "eigen", "gsl"]) + \
                                    ["src", "src/third"]
              )
]
#
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
NUM_MODULE = [
    Extension('{}._gsl'.format(NAME),
              sources = [WRAPPER_PYGSL_C] + getfn(PY_GSL, 'src/third'),
              include_dirs = ['src', 'src/third', 'src/third/gsl', 'src/third/gsl/specfunc']
              ),
    Extension('{}._boostmath'.format(NAME),
              sources = [WRAPPER_PYBOOSTMATH_CPP],
              include_dirs = ['src', 'src/third']
              )
]
ASSOTESTS_MODULE = [
    Extension('{}._assoTests'.format(NAME),
              sources = [WRAPPER_ASSOC_CPP] + ASSOC_CPP + getfn(LIB_GSL, 'src/third'),
              extra_compile_args = ["-O3", "-march=native", "-std=c++11"],
              libraries = libs,
              library_dirs = [],
              include_dirs = ["src/assoTests", "src/third", "src/third/gsl"]
              )
]

setup(name = NAME,
    version = VERSION,
    description = "Gao Wang's statgen library",
    author = "Gao Wang",
    packages = [NAME, NAME + ".egglib"],
    package_dir = {NAME:'src'},
    package_data = {NAME + ".egglib":['apps.conf']},
    cmdclass = {'build_py': build_py},
    ext_modules = CSTATGEN_MODULE + EGGLIB_MODULE + NUM_MODULE + ASSOTESTS_MODULE
)
