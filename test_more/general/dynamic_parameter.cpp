/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-18 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

# include <cppad/cppad.hpp>

namespace {
	//
	// optimize
	bool optimize(void)
	{	bool ok = true;
		using CppAD::AD;
		using CppAD::NearEqual;

		// domain space vector
		size_t nx = 1;
		CPPAD_TESTVECTOR(AD<double>) ax(nx);
		ax[0] = 1.;

		// no abort operator index
		size_t abort_op_index = 0;

		// independent dynamic parameter vector
		size_t np = 1;
		CPPAD_TESTVECTOR(AD<double>) ap(np);
		ap[0] = 2.0;

		// declare independent variables and starting recording
		CppAD::Independent(ax, abort_op_index, ap);

		// non-dynamic parameter that happends to be equal dynamic paramter
		AD<double> ap1 = 2.0;

		// range space vector
		size_t ny = 2;
		CPPAD_TESTVECTOR(AD<double>) ay(ny);
		ay[0] = ax[0] * ap[0]; // dynamic parameter
		ay[1] = ax[0] * ap1;   // non-dynamic parameter

		// create f: x -> y and stop tape recording
		CppAD::ADFun<double> f(ax, ay);

		// optimize
		f.optimize();

		// change value of dynamic parameters and run zero order forward
		CPPAD_TESTVECTOR(double) p(np), x(nx), y(ny);
		p[0] = 5.0;
		f.dynamic_parameter(p);
		x[0] = 3.0;
		y    = f.Forward(0, x);
		ok  &= y[0] == x[0] * p[0];
		ok  &= y[1] == x[0] * Value(ap1);

		return ok;
	}
}

bool dynamic_parameter(void)
{	bool ok = true;
	ok     &= optimize();

	return ok;
}
