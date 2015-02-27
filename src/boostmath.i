// $File: boostmath.i$
// $LastChangedDate:  $
// $Rev:  $
// This file is part of the ExPOWER program
// Copyright (c) 2012, Gao Wang <ewanggao@gmail.com>
// GNU General Public License (http://www.gnu.org/licenses/gpl.html)

%module boostmath
%{
#include "boost/math/distributions/non_central_chi_squared.hpp"
#include "boost/math/distributions/non_central_t.hpp"
%}
%inline
%{
double boost_cdf_non_central_chisq_Pinv(double x, double ncp, double df) {
  using boost::math::non_central_chi_squared;
  return cdf(complement(non_central_chi_squared(df, ncp), x));
}
double boost_cdf_non_central_chisq_Qinv(double x, double ncp, double df) {
  using boost::math::non_central_chi_squared;
  return quantile(complement(non_central_chi_squared(df, ncp), x));
}
double boost_cdf_non_central_tdist_Pinv(double x, double ncp, double df) {
  using boost::math::non_central_t;
  return cdf(complement(non_central_t(df, ncp), x));
}

%}
