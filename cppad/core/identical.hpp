# ifndef CPPAD_CORE_IDENTICAL_HPP
# define CPPAD_CORE_IDENTICAL_HPP

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-18 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

# include <cppad/core/define.hpp>

namespace CppAD { // BEGIN_CPPAD_NAMESPACE
/*!
\file identical.hpp
Check if certain properties is true for any possible AD tape play back.
*/

// ---------------------------------------------------------------------------
/*!
Determine if an AD<Base> object is a parameter, and could never have
a different value during any tape playback.

An AD<Base> object \c x is identically a parameter if and only if
all of the objects in the following chain are parameters:
\code
	x , x.value , x.value.value , ...
\endcode
In such a case, the value of the object will always be the same
no matter what the independent variable values are at any level.

\param x
values that we are checking for identically a pamameter.

\return
returns true iff \c x is identically a parameter.
*/
template <class Base>
CPPAD_INLINE_FRIEND_TEMPLATE_FUNCTION
bool IdenticalPar(const AD<Base> &x)
{	if( x.tape_id_ == 0 )
		return IdenticalPar(x.value_);
	//
	size_t thread = size_t(x.tape_id_ % CPPAD_MAX_NUM_THREADS);
	bool result   = x.tape_id_ != *AD<Base>::tape_id_ptr(thread);
	result       |= (x.taddr_ == 0) & (x.dynamic_id_ == 0);
	if( result )
		result = IdenticalPar(x.value_);
	//
	return result;
}
// Zero ==============================================================
/*!
Determine if an AD<Base> is equal to zero,
and must be equal zero during any tape playback.

\param x
object that we are checking.

\return
returns true if and only if
\c x is equals zero and is identically a parameter \ref CppAD::IdenticalPar.
*/
template <class Base>
CPPAD_INLINE_FRIEND_TEMPLATE_FUNCTION
bool IdenticalZero(const AD<Base> &x)
{	if( x.tape_id_ == 0 )
		return IdenticalZero(x.value_);
	//
	size_t thread = size_t(x.tape_id_ % CPPAD_MAX_NUM_THREADS);
	bool result   = x.tape_id_ != *AD<Base>::tape_id_ptr(thread);
	result       |= (x.taddr_ == 0) & (x.dynamic_id_ == 0);
	if( result )
		result = IdenticalZero(x.value_);
	//
	return result;
}
// One ==============================================================
/*!
Determine if an AD<Base> is equal to one,
and must be equal one during any tape playback.

\param x
object that we are checking.

\return
returns true if and only if
\c x is equals one and is identically a parameter \ref CppAD::IdenticalPar.
*/
template <class Base>
CPPAD_INLINE_FRIEND_TEMPLATE_FUNCTION
bool IdenticalOne(const AD<Base> &x)
{	if( x.tape_id_ == 0 )
		return IdenticalOne(x.value_);
	//
	size_t thread = size_t(x.tape_id_ % CPPAD_MAX_NUM_THREADS);
	bool result   = x.tape_id_ != *AD<Base>::tape_id_ptr(thread);
	result       |= (x.taddr_ == 0) & (x.dynamic_id_ == 0);
	if( result )
		result = IdenticalOne(x.value_);
	//
	return result;
}
// Equal ===================================================================
/*!
Determine if two AD<Base> objects are equal,
and must be equal during any tape playback.

\param x
first of two objects we are checking for equal.

\param y
second of two objects we are checking for equal.

\return
returns true if and only if
the arguments are equal and both identically parameters \ref CppAD::IdenticalPar.
*/
template <class Base>
CPPAD_INLINE_FRIEND_TEMPLATE_FUNCTION
bool IdenticalEqualPar
(const AD<Base> &x, const AD<Base> &y)
{	bool x_constant = x.tape_id_ == 0;
	bool y_constant = y.tape_id_ == 0;
	if( x_constant & y_constant )
		return IdenticalEqualPar(x.value_, y.value_);
	//
	size_t x_thread = size_t(x.tape_id_ % CPPAD_MAX_NUM_THREADS);
	x_constant      = x.tape_id_ != *AD<Base>::tape_id_ptr(x_thread);
	x_constant     |= (x.taddr_ == 0) & (x.dynamic_id_ == 0);
	//
	size_t y_thread = size_t(y.tape_id_ % CPPAD_MAX_NUM_THREADS);
	y_constant      = y.tape_id_ != *AD<Base>::tape_id_ptr(y_thread);
	y_constant     |= (y.taddr_ == 0) & (y.dynamic_id_ == 0);
	//
	if( x_constant & y_constant )
		return IdenticalEqualPar(x.value_, y.value_);
	//
	return false;
}
// ==========================================================================

} // END_CPPAD_NAMESPACE
# endif
