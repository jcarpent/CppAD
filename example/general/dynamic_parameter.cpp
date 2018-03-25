/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-18 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

/*
$begin dynamic_parameter.cpp$$
$spell
	Cpp
$$

$section Forward Mode: Example and Test$$

$code
$srcfile%example/general/dynamic_parameter.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>

bool dynamic_parameter(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	double eps10 = 10. * std::numeric_limits<double>::epsilon();

	// domain space vector
	size_t nx = 2;
	CPPAD_TESTVECTOR(AD<double>) ax(nx);
	ax[0] = 0.;
	ax[1] = 1.;

	// no abort operator index
	size_t abort_op_index = 0;

	// independent dynamic parameter vector
	size_t np = 2;
	CPPAD_TESTVECTOR(AD<double>) ap(np);
	ap[0] = 2.0;
	ap[1] = 3.0;

	// declare independent variables and starting recording
	CppAD::Independent(ax, abort_op_index, ap);

	// dependent dynamic paramter
	AD<double> ap2 = ap[0] * ap[1];

	// range space vector
	size_t ny = 2;
	CPPAD_TESTVECTOR(AD<double>) ay(ny);
	ay[0] = ax[0] * ap[0];
	ay[1] = ax[1] * ap2;

	// create f: x -> y and stop tape recording
	CppAD::ADFun<double> f(ax, ay);

	// zero order forward mode using value of dynamic parameter during taping
	CPPAD_TESTVECTOR(double) x0(nx), y0(ny);
	x0[0] = 3.;
	x0[1] = 4.;
	y0    = f.Forward(0, x0);
	ok  &= NearEqual(y0[0] , x0[0] * Value(ap[0]), eps10, eps10);
	ok  &= NearEqual(y0[1] , x0[1] * Value(ap[0] * ap[1]), eps10, eps10);

	// change value of dynamic parameters and re-run zero order forward
	CPPAD_TESTVECTOR(double) p(np);
	p[0] = 5.0;
	p[1] = 6.0;
	f.dynamic_parameter(p);
	y0    = f.Forward(0, x0);
	ok  &= NearEqual(y0[0] , x0[0] * p[0], eps10, eps10);
	ok  &= NearEqual(y0[1] , x0[1] * p[0] * p[1], eps10, eps10);

	return ok;
}
// END C++
