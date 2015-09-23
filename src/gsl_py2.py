# This file was automatically generated by SWIG (http://www.swig.org).
# Version 2.0.12
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.




"""
This module exposes the following GSL (GUN Scientific Library) functions used
by simuPOP to the user interface. Although more functions may be added from time
to time, this module is not intended to become a complete wrapper for GSL. Please
refer to the GSL reference manual (http://www.gnu.org/software/gsl/manual/html_node/)
for details about these functions. Note that random number generation functions
are wrapped into the simuPOP.RNG class.

- ``gsl_cdf_gaussian_P(x, sigma)``
- ``gsl_cdf_gaussian_Q(x, sigma)``
- ``gsl_cdf_gaussian_Pinv(P, sigma)``
- ``gsl_cdf_gaussian_Qinv(Q, sigma)``
- ``gsl_cdf_ugaussian_P(x)``
- ``gsl_cdf_ugaussian_Q(x)``
- ``gsl_cdf_ugaussian_Pinv(P)``
- ``gsl_cdf_ugaussian_Qinv(Q)``
- ``gsl_cdf_tdist_P(x, nu)``
- ``gsl_cdf_tdist_Q(x, nu)``
- ``gsl_cdf_tdist_Pinv(P, nu)``
- ``gsl_cdf_tdist_Qinv(Q, nu)``
- ``gsl_cdf_exponential_P(x, mu)``
- ``gsl_cdf_exponential_Q(x, mu)``
- ``gsl_cdf_exponential_Pinv(P, mu)``
- ``gsl_cdf_exponential_Qinv(Q, mu)``
- ``gsl_cdf_chisq_P(x, nu)``
- ``gsl_cdf_chisq_Q(x, nu)``
- ``gsl_cdf_chisq_Pinv(P, nu)``
- ``gsl_cdf_chisq_Qinv(Q, nu)``
- ``gsl_cdf_gamma_P(x, a, b)``
- ``gsl_cdf_gamma_Q(x, a, b)``
- ``gsl_cdf_gamma_Pinv(P, a, b)``
- ``gsl_cdf_gamma_Qinv(Q, a, b)``
- ``gsl_ran_gamma_pdf(x, a, b)``
- ``gsl_cdf_beta_P(x, a, b)``
- ``gsl_cdf_beta_Q(x, a, b)``
- ``gsl_cdf_beta_Pinv(P, a, b)``
- ``gsl_cdf_beta_Qinv(Q, a, b)``
- ``gsl_ran_beta_pdf(x, a, b)``
- ``gsl_cdf_binomial_P(k, p, n)``
- ``gsl_cdf_binomial_Q(k, p, n)``
- ``gsl_ran_binomial_pdf(k, p, n)``
- ``gsl_cdf_poisson_P(k, mu)``
- ``gsl_cdf_poisson_Q(k, mu)``
- ``gsl_ran_poisson_pdf(k, mu)``

"""


from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_gsl', [dirname(__file__)])
        except ImportError:
            import _gsl
            return _gsl
        if fp is not None:
            try:
                _mod = imp.load_module('_gsl', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _gsl = swig_import_helper()
    del swig_import_helper
else:
    import _gsl
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0



def my_error_handler(*args, **kwargs):
  return _gsl.my_error_handler(*args, **kwargs)
my_error_handler = _gsl.my_error_handler

def initialize():
  return _gsl.initialize()
initialize = _gsl.initialize

def gsl_ran_gamma_pdf(*args, **kwargs):
  return _gsl.gsl_ran_gamma_pdf(*args, **kwargs)
gsl_ran_gamma_pdf = _gsl.gsl_ran_gamma_pdf

def gsl_ran_tdist_pdf(*args, **kwargs):
  return _gsl.gsl_ran_tdist_pdf(*args, **kwargs)
gsl_ran_tdist_pdf = _gsl.gsl_ran_tdist_pdf

def gsl_log1p(*args, **kwargs):
  return _gsl.gsl_log1p(*args, **kwargs)
gsl_log1p = _gsl.gsl_log1p

def gsl_ran_binomial_pdf(*args, **kwargs):
  return _gsl.gsl_ran_binomial_pdf(*args, **kwargs)
gsl_ran_binomial_pdf = _gsl.gsl_ran_binomial_pdf

def gsl_ran_beta_pdf(*args, **kwargs):
  return _gsl.gsl_ran_beta_pdf(*args, **kwargs)
gsl_ran_beta_pdf = _gsl.gsl_ran_beta_pdf

def gsl_ran_poisson_pdf(*args, **kwargs):
  return _gsl.gsl_ran_poisson_pdf(*args, **kwargs)
gsl_ran_poisson_pdf = _gsl.gsl_ran_poisson_pdf

def gsl_cdf_gaussian_P(*args, **kwargs):
  return _gsl.gsl_cdf_gaussian_P(*args, **kwargs)
gsl_cdf_gaussian_P = _gsl.gsl_cdf_gaussian_P

def gsl_cdf_gaussian_Q(*args, **kwargs):
  return _gsl.gsl_cdf_gaussian_Q(*args, **kwargs)
gsl_cdf_gaussian_Q = _gsl.gsl_cdf_gaussian_Q

def gsl_cdf_gaussian_Pinv(*args, **kwargs):
  return _gsl.gsl_cdf_gaussian_Pinv(*args, **kwargs)
gsl_cdf_gaussian_Pinv = _gsl.gsl_cdf_gaussian_Pinv

def gsl_cdf_gaussian_Qinv(*args, **kwargs):
  return _gsl.gsl_cdf_gaussian_Qinv(*args, **kwargs)
gsl_cdf_gaussian_Qinv = _gsl.gsl_cdf_gaussian_Qinv

def gsl_cdf_ugaussian_P(*args, **kwargs):
  return _gsl.gsl_cdf_ugaussian_P(*args, **kwargs)
gsl_cdf_ugaussian_P = _gsl.gsl_cdf_ugaussian_P

def gsl_cdf_ugaussian_Q(*args, **kwargs):
  return _gsl.gsl_cdf_ugaussian_Q(*args, **kwargs)
gsl_cdf_ugaussian_Q = _gsl.gsl_cdf_ugaussian_Q

def gsl_cdf_ugaussian_Pinv(*args, **kwargs):
  return _gsl.gsl_cdf_ugaussian_Pinv(*args, **kwargs)
gsl_cdf_ugaussian_Pinv = _gsl.gsl_cdf_ugaussian_Pinv

def gsl_cdf_ugaussian_Qinv(*args, **kwargs):
  return _gsl.gsl_cdf_ugaussian_Qinv(*args, **kwargs)
gsl_cdf_ugaussian_Qinv = _gsl.gsl_cdf_ugaussian_Qinv

def gsl_cdf_tdist_P(*args, **kwargs):
  return _gsl.gsl_cdf_tdist_P(*args, **kwargs)
gsl_cdf_tdist_P = _gsl.gsl_cdf_tdist_P

def gsl_cdf_tdist_Q(*args, **kwargs):
  return _gsl.gsl_cdf_tdist_Q(*args, **kwargs)
gsl_cdf_tdist_Q = _gsl.gsl_cdf_tdist_Q

def gsl_cdf_tdist_Pinv(*args, **kwargs):
  return _gsl.gsl_cdf_tdist_Pinv(*args, **kwargs)
gsl_cdf_tdist_Pinv = _gsl.gsl_cdf_tdist_Pinv

def gsl_cdf_tdist_Qinv(*args, **kwargs):
  return _gsl.gsl_cdf_tdist_Qinv(*args, **kwargs)
gsl_cdf_tdist_Qinv = _gsl.gsl_cdf_tdist_Qinv

def gsl_cdf_exponential_P(*args, **kwargs):
  return _gsl.gsl_cdf_exponential_P(*args, **kwargs)
gsl_cdf_exponential_P = _gsl.gsl_cdf_exponential_P

def gsl_cdf_exponential_Q(*args, **kwargs):
  return _gsl.gsl_cdf_exponential_Q(*args, **kwargs)
gsl_cdf_exponential_Q = _gsl.gsl_cdf_exponential_Q

def gsl_cdf_exponential_Pinv(*args, **kwargs):
  return _gsl.gsl_cdf_exponential_Pinv(*args, **kwargs)
gsl_cdf_exponential_Pinv = _gsl.gsl_cdf_exponential_Pinv

def gsl_cdf_exponential_Qinv(*args, **kwargs):
  return _gsl.gsl_cdf_exponential_Qinv(*args, **kwargs)
gsl_cdf_exponential_Qinv = _gsl.gsl_cdf_exponential_Qinv

def gsl_cdf_chisq_P(*args, **kwargs):
  return _gsl.gsl_cdf_chisq_P(*args, **kwargs)
gsl_cdf_chisq_P = _gsl.gsl_cdf_chisq_P

def gsl_cdf_chisq_Q(*args, **kwargs):
  return _gsl.gsl_cdf_chisq_Q(*args, **kwargs)
gsl_cdf_chisq_Q = _gsl.gsl_cdf_chisq_Q

def gsl_cdf_chisq_Pinv(*args, **kwargs):
  return _gsl.gsl_cdf_chisq_Pinv(*args, **kwargs)
gsl_cdf_chisq_Pinv = _gsl.gsl_cdf_chisq_Pinv

def gsl_cdf_chisq_Qinv(*args, **kwargs):
  return _gsl.gsl_cdf_chisq_Qinv(*args, **kwargs)
gsl_cdf_chisq_Qinv = _gsl.gsl_cdf_chisq_Qinv

def gsl_cdf_gamma_P(*args, **kwargs):
  return _gsl.gsl_cdf_gamma_P(*args, **kwargs)
gsl_cdf_gamma_P = _gsl.gsl_cdf_gamma_P

def gsl_cdf_gamma_Q(*args, **kwargs):
  return _gsl.gsl_cdf_gamma_Q(*args, **kwargs)
gsl_cdf_gamma_Q = _gsl.gsl_cdf_gamma_Q

def gsl_cdf_gamma_Pinv(*args, **kwargs):
  return _gsl.gsl_cdf_gamma_Pinv(*args, **kwargs)
gsl_cdf_gamma_Pinv = _gsl.gsl_cdf_gamma_Pinv

def gsl_cdf_gamma_Qinv(*args, **kwargs):
  return _gsl.gsl_cdf_gamma_Qinv(*args, **kwargs)
gsl_cdf_gamma_Qinv = _gsl.gsl_cdf_gamma_Qinv

def gsl_cdf_binomial_P(*args, **kwargs):
  return _gsl.gsl_cdf_binomial_P(*args, **kwargs)
gsl_cdf_binomial_P = _gsl.gsl_cdf_binomial_P

def gsl_cdf_binomial_Q(*args, **kwargs):
  return _gsl.gsl_cdf_binomial_Q(*args, **kwargs)
gsl_cdf_binomial_Q = _gsl.gsl_cdf_binomial_Q

def gsl_cdf_beta_P(*args, **kwargs):
  return _gsl.gsl_cdf_beta_P(*args, **kwargs)
gsl_cdf_beta_P = _gsl.gsl_cdf_beta_P

def gsl_cdf_beta_Q(*args, **kwargs):
  return _gsl.gsl_cdf_beta_Q(*args, **kwargs)
gsl_cdf_beta_Q = _gsl.gsl_cdf_beta_Q

def gsl_cdf_beta_Pinv(*args, **kwargs):
  return _gsl.gsl_cdf_beta_Pinv(*args, **kwargs)
gsl_cdf_beta_Pinv = _gsl.gsl_cdf_beta_Pinv

def gsl_cdf_beta_Qinv(*args, **kwargs):
  return _gsl.gsl_cdf_beta_Qinv(*args, **kwargs)
gsl_cdf_beta_Qinv = _gsl.gsl_cdf_beta_Qinv

def gsl_cdf_poisson_P(*args, **kwargs):
  return _gsl.gsl_cdf_poisson_P(*args, **kwargs)
gsl_cdf_poisson_P = _gsl.gsl_cdf_poisson_P

def gsl_cdf_poisson_Q(*args, **kwargs):
  return _gsl.gsl_cdf_poisson_Q(*args, **kwargs)
gsl_cdf_poisson_Q = _gsl.gsl_cdf_poisson_Q
# This file is compatible with both classic and new-style classes.


