# ifndef CPPAD_CORE_DYNAMIC_HPP
# define CPPAD_CORE_DYNAMIC_HPP

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-18 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

/*
$begin dynamic_parameter$$
$spell
	ind
	const
$$

$section Change Value of Dynamic Parameters$$

$head Under Construction$$

$head Syntax$$
$icode%f%.dynamic_parameter(%ind_dynamic_par%)%$$

$head Dynamic Parameters$$
The dynamic $cref/parameters/glossary/Parameter/$$ are the
independent parameters, and any parameter that depend on the
independent dynamic parameters.
The independent dynamic parameters are specified by the
$cref/ind_dynamic_par/Independent/ind_dynamic_par/$$ vector
in the call to $code Independent$$ that started
the recording for $icode f$$.

$head Purpose$$
This is like a $cref forward_zero$$ operation except instead
of changing the value of the
$cref/variables/glossary/Variable/$$, it changes
the value of the dynamic parameters.

$head f$$
The object $icode f$$ has prototype
$codei%
	ADFun<%Base%> %f%
%$$
Note that the $cref ADFun$$ object $icode f$$ is not $code const$$.
After this call all of the dynamic parameters in $icode f$$
have been changed to the value corresponding to
the new independent dynamic parameter vector.

$head ind_dynamic_par$$
This argument has prototype
$codei%
	const %VectorBase%& %ind_dynamic_par%
%$$
(see $icode VectorBase$$ below).
It specifies the new value for the independent dynamic parameters.
It size must be the same as the size of
$cref/ind_dynamic_par/Independent/ind_dynamic_par/$$
in the call to $code Independent$$ that started
the recording for $icode f$$.

$head VectorBase$$
The type $icode VectorBase$$ must be a $cref SimpleVector$$ class with
$cref/elements of type/SimpleVector/Elements of Specified Type/$$
$icode Base$$.

$children%
	example/general/dynamic_parameter.cpp
%$$
$head Example$$
The file $cref dynamic_parameter.cpp$$
contains an example and test of this operation.
It returns true if it succeeds and false otherwise.

$end
*/

namespace CppAD { // BEGIN_CPPAD_NAMESPACE
/*!
\file dynamic.hpp
User interface to ADFun dynamic_parameter member function.
*/

/*!
Change the dynamic parameters in this ADFun object

\param ind_dynamic_par
is the vector of new values for the independent dynamic parameters.

*/

template <typename Base>
template <typename VectorBase>
void ADFun<Base>::dynamic_parameter(const VectorBase& ind_dynamic_par)
{	size_t num_ind_dynamic_par = play_.num_ind_dynamic_par();
	CPPAD_ASSERT_UNKNOWN(
		size_t(ind_dynamic_par.size()) == num_ind_dynamic_par
	);
	CPPAD_ASSERT_UNKNOWN( num_ind_dynamic_par < play_.num_par_rec() );

	// check VectorBase is Simple Vector class with Base elements
	CheckSimpleVector<Base, VectorBase>();

	// get pointer to parameter vector in play_
	Base* parameter = play_.parameter();

	// set the independent dynamic parameters
	// (skip zero parameter at beginning)
	CPPAD_ASSERT_UNKNOWN( parameter[0] == Base(0) );
	for(size_t i = 0; i < num_ind_dynamic_par; ++i)
		parameter[1 + i] = ind_dynamic_par[i];

	// compute the dependent dynamic parameters
	dynamic_sweep( &play_ );

	return;
}


} // END_CPPAD_NAMESPACE
# endif
