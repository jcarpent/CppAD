// $Id$
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-15 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

/*
$begin atomic_norm_sq.cpp$$
$spell
	sq
$$

$section Euclidean Norm Squared: Example and Test$$

$head Theory$$
This example demonstrates using $cref atomic_base$$
to define the operation
$latex f : \B{R}^n \rightarrow \B{R}^m$$ where
$latex n = 2$$, $latex m = 1$$, where
$latex \[
	f(x) =  x_0^2 + x_1^2
\] $$

$nospell

$head Start Class Definition$$
$codep */
# include <cppad/cppad.hpp>
namespace {           // isolate items below to this file
using CppAD::vector;  // abbreviate as vector
//
class atomic_norm_sq : public CppAD::atomic_base<double> {
/* $$
$head Constructor $$
$codep */
	public:
	// constructor (could use const char* for name)
	atomic_norm_sq(const std::string& name) :
	// this example uses boolean sparsity patterns
	CppAD::atomic_base<double>(name, atomic_base<double>::bool_sparsity_enum)
	{ }
	private:
/* $$
$head forward$$
$codep */
	// forward mode routine called by CppAD
	virtual bool forward(
		size_t                    p ,
		size_t                    q ,
		const vector<bool>&      vx ,
		      vector<bool>&      vy ,
		const vector<double>&    tx ,
		      vector<double>&    ty
	)
	{	size_t n = tx.size() / (q+1);
		size_t m = ty.size() / (q+1);
		assert( n == 2 );
		assert( m == 1 );
		assert( p <= q );

		// return flag
		bool ok = q <= 1;

		// Variable information must always be implemented.
		// y_0 is a variable if and only if x_0 or x_1 is a variable.
		if( vx.size() > 0 )
			vy[0] = vx[0] | vx[1];

		// Order zero forward mode must always be implemented.
		// y^0 = f( x^0 )
		double x_00 = tx[ 0*(q+1) + 0];        // x_0^0
		double x_10 = tx[ 1*(q+1) + 0];        // x_10
		double f = x_00 * x_00 + x_10 * x_10;  // f( x^0 )
		if( p <= 0 )
			ty[0] = f;   // y_0^0
		if( q <= 0 )
			return ok;
		assert( vx.size() == 0 );

		// Order one forward mode.
		// This case needed if first order forward mode is used.
		// y^1 = f'( x^0 ) x^1
		double x_01 = tx[ 0*(q+1) + 1];   // x_0^1
		double x_11 = tx[ 1*(q+1) + 1];   // x_1^1
		double fp_0 = 2.0 * x_00;         // partial f w.r.t x_0^0
		double fp_1 = 2.0 * x_10;         // partial f w.r.t x_1^0
		if( p <= 1 )
			ty[1] = fp_0 * x_01 + fp_1 * x_11; // f'( x^0 ) * x^1
		if( q <= 1 )
			return ok;

		// Assume we are not using forward mode with order > 1
		assert( ! ok );
		return ok;
	}
/* $$
$head reverse$$
$codep */
	// reverse mode routine called by CppAD
	virtual bool reverse(
		size_t                    q ,
		const vector<double>&    tx ,
		const vector<double>&    ty ,
		      vector<double>&    px ,
		const vector<double>&    py
	)
	{	size_t n = tx.size() / (q+1);
		size_t m = ty.size() / (q+1);
		assert( px.size() == n * (q+1) );
		assert( py.size() == m * (q+1) );
		assert( n == 2 );
		assert( m == 1 );
		bool ok = q <= 1;

		double fp_0, fp_1;
		switch(q)
		{	case 0:
			// This case needed if first order reverse mode is used
			// F ( {x} ) = f( x^0 ) = y^0
			fp_0  =  2.0 * tx[0];  // partial F w.r.t. x_0^0
			fp_1  =  2.0 * tx[1];  // partial F w.r.t. x_0^1
			px[0] = py[0] * fp_0;; // partial G w.r.t. x_0^0
			px[1] = py[0] * fp_1;; // partial G w.r.t. x_0^1
			assert(ok);
			break;

			default:
			// Assume we are not using reverse with order > 1 (q > 0)
			assert(!ok);
		}
		return ok;
	}
/* $$
$head for_sparse_jac$$
$codep */
	// forward Jacobian bool sparsity routine called by CppAD
	virtual bool for_sparse_jac(
		size_t                                p ,
		const vector<bool>&                   r ,
		      vector<bool>&                   s )
	{	// This function needed if using f.ForSparseJac
		size_t n = r.size() / p;
		size_t m = s.size() / p;
		assert( n == 2 );
		assert( m == 1 );

		// sparsity for S(x) = f'(x) * R
		// where f'(x) = 2 * [ x_0, x_1 ]
		for(size_t j = 0; j < p; j++)
		{	s[j] = false;
			for(size_t i = 0; i < n; i++)
				s[j] |= r[i * p + j];
		}
		return true;
	}
/* $$
$head rev_sparse_jac$$
$codep */
	// reverse Jacobian bool sparsity routine called by CppAD
	virtual bool rev_sparse_jac(
		size_t                                p  ,
		const vector<bool>&                   rt ,
		      vector<bool>&                   st )
	{	// This function needed if using RevSparseJac or optimize
		size_t n = st.size() / p;
		size_t m = rt.size() / p;
		assert( n == 2 );
		assert( m == 1 );

		// sparsity for S(x)^T = f'(x)^T * R^T
		// where f'(x)^T = 2 * [ x_0, x_1]^T
		for(size_t j = 0; j < p; j++)
			for(size_t i = 0; i < n; i++)
				st[i * p + j] = rt[j];

		return true;
	}
/* $$
$head rev_sparse_hes$$
$codep */
	// reverse Hessian bool sparsity routine called by CppAD
	virtual bool rev_sparse_hes(
		const vector<bool>&                   vx,
		const vector<bool>&                   s ,
		      vector<bool>&                   t ,
		size_t                                p ,
		const vector<bool>&                   r ,
		const vector<bool>&                   u ,
		      vector<bool>&                   v )
	{	// This function needed if using RevSparseHes
		size_t m = s.size();
		size_t n = t.size();
		assert( r.size() == n * p );
		assert( u.size() == m * p );
		assert( v.size() == n * p );
		assert( n == 2 );
		assert( m == 1 );

		// There are no cross term second derivatives for this case,
		// so it is not necessary to use vx.

		// sparsity for T(x) = S(x) * f'(x)
		t[0] = s[0];
		t[1] = s[0];

		// V(x) = f'(x)^T * g''(y) * f'(x) * R  +  g'(y) * f''(x) * R
		// U(x) = g''(y) * f'(x) * R
		// S(x) = g'(y)

		// back propagate the sparsity for U
		size_t j;
		for(j = 0; j < p; j++)
			for(size_t i = 0; i < n; i++)
				v[ i * p + j] = u[j];

		// include forward Jacobian sparsity in Hessian sparsity
		// sparsity for g'(y) * f''(x) * R  (Note f''(x) has same sparsity
		// as the identity matrix)
		if( s[0] )
		{	for(j = 0; j < p; j++)
				for(size_t i = 0; i < n; i++)
					v[ i * p + j] |= r[ i * p + j];
		}

		return true;
	}
/* $$
$head End Class Definition$$
$codep */
}; // End of atomic_norm_sq class
}  // End empty namespace

/* $$
$head Use Atomic Function$$
$codep */
bool norm_sq(void)
{	bool ok = true;
	using CppAD::AD;
	using CppAD::NearEqual;
	double eps = 10. * CppAD::numeric_limits<double>::epsilon();
/* $$
$subhead Constructor$$
$codep */
	// --------------------------------------------------------------------
	// Create the atomic reciprocal object
	atomic_norm_sq afun("atomic_norm_sq");
/* $$
$subhead Recording$$
$codep */
	// Create the function f(x)
	//
	// domain space vector
	size_t  n  = 2;
	double  x0 = 0.25;
	double  x1 = 0.75;
	vector< AD<double> > ax(n);
	ax[0]      = x0;
	ax[1]      = x1;

	// declare independent variables and start tape recording
	CppAD::Independent(ax);

	// range space vector
	size_t m = 1;
	vector< AD<double> > ay(m);

	// call user function and store norm_sq(x) in au[0]
	afun(ax, ay);        // y_0 = x_0 * x_0 + x_1 * x_1

	// create f: x -> y and stop tape recording
	CppAD::ADFun<double> f;
	f.Dependent (ax, ay);
/* $$
$subhead forward$$
$codep */
	// check function value
	double check = x0 * x0 + x1 * x1;
	ok &= NearEqual( Value(ay[0]) , check,  eps, eps);

	// check zero order forward mode
	size_t q;
	vector<double> x_q(n), y_q(m);
	q      = 0;
	x_q[0] = x0;
	x_q[1] = x1;
	y_q    = f.Forward(q, x_q);
	ok &= NearEqual(y_q[0] , check,  eps, eps);

	// check first order forward mode
	q      = 1;
	x_q[0] = 0.3;
	x_q[1] = 0.7;
	y_q    = f.Forward(q, x_q);
	check  = 2.0 * x0 * x_q[0] + 2.0 * x1 * x_q[1];
	ok &= NearEqual(y_q[0] , check,  eps, eps);

/* $$
$subhead reverse$$
$codep */
	// first order reverse mode
	q     = 1;
	vector<double> w(m), dw(n * q);
	w[0]  = 1.;
	dw    = f.Reverse(q, w);
	check = 2.0 * x0;
	ok &= NearEqual(dw[0] , check,  eps, eps);
	check = 2.0 * x1;
	ok &= NearEqual(dw[1] , check,  eps, eps);
/* $$
$subhead for_sparse_jac$$
$codep */
	// forward mode sparstiy pattern
	size_t p = n;
	CppAD::vectorBool r1(n * p), s1(m * p);
	r1[0] = true;  r1[1] = false; // sparsity pattern identity
	r1[2] = false; r1[3] = true;
	//
	s1    = f.ForSparseJac(p, r1);
	ok  &= s1[0] == true;  // f[0] depends on x[0]
	ok  &= s1[1] == true;  // f[0] depends on x[1]
/* $$
$subhead rev_sparse_jac$$
$codep */
	// reverse mode sparstiy pattern
	q = m;
	CppAD::vectorBool s2(q * m), r2(q * n);
	s2[0] = true;          // compute sparsity pattern for f[0]
	//
	r2    = f.RevSparseJac(q, s2);
	ok  &= r2[0] == true;  // f[0] depends on x[0]
	ok  &= r2[1] == true;  // f[0] depends on x[1]
/* $$
$subhead rev_sparse_hes$$
$codep */
	// Hessian sparsity (using previous ForSparseJac call)
	CppAD::vectorBool s3(m), h(p * n);
	s3[0] = true;        // compute sparsity pattern for f[0]
	//
	h     = f.RevSparseHes(p, s3);
	ok  &= h[0] == true;  // partial of f[0] w.r.t. x[0],x[0] is non-zero
	ok  &= h[1] == false; // partial of f[0] w.r.t. x[0],x[1] is zero
	ok  &= h[2] == false; // partial of f[0] w.r.t. x[1],x[0] is zero
	ok  &= h[3] == true;  // partial of f[0] w.r.t. x[1],x[1] is non-zero
	//
	return ok;
}
/* $$
$$ $comment end nospell$$
$end
*/
