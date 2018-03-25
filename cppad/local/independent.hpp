# ifndef CPPAD_LOCAL_INDEPENDENT_HPP
# define CPPAD_LOCAL_INDEPENDENT_HPP

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-18 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
namespace CppAD { namespace local { //  BEGIN_CPPAD_LOCAL_NAMESPACE
/*
\file local/independent.hpp
Implement the declaration of the independent variables
*/

/*!
Implementation of the declaration of independent variables (in local namespace).

\tparam VectorAD
This is simple vector type with elements of type AD<Base>.

\param x
Vector of the independent variablerd.

\param abort_op_index
operator index at which execution will be aborted (during  the recording
of operations). The value zero corresponds to not aborting (will not match).

\param ind_dynamic_par
vector of parameters that can be changed after the function is recorded.
*/
template <typename Base>
template <typename VectorAD>
void ADTape<Base>::Independent(
	VectorAD&       x                 ,
	size_t          abort_op_index    ,
	VectorAD&       ind_dynamic_par )
{
	// check VectorAD is Simple Vector class with AD<Base> elements
	CheckSimpleVector< AD<Base>, VectorAD>();

	// dimension of the domain space
	size_t n = x.size();
	CPPAD_ASSERT_KNOWN(
		n > 0,
		"Indepdendent: the argument vector x has zero size"
	);
	CPPAD_ASSERT_UNKNOWN( Rec_.num_var_rec() == 0 );
	CPPAD_ASSERT_UNKNOWN( Rec_.num_par_rec() == 0 );

	// first set the abort index
	// and size of independent dynamic parameter vector
	Rec_.set_abort_op_index(abort_op_index);
	size_t num_ind_dynamic_par = size_t( ind_dynamic_par.size() );
	Rec_.set_num_ind_dynamic_par( num_ind_dynamic_par );

	// mark the beginning of the tape and skip the first variable index
	// (zero) because parameters use taddr zero
	CPPAD_ASSERT_NARG_NRES(BeginOp, 1, 1);
	Rec_.PutOp(BeginOp);
	Rec_.PutArg(0);

	// skip the first Parameter index,
	// and follow it by the independent dynamic parameters
	Rec_.PutPar( Base(0) );
	for(size_t i = 0; i < num_ind_dynamic_par; ++i)
	{	// tape_id
		ind_dynamic_par[i].tape_id_ = id_;
		// dynamic_id
		ind_dynamic_par[i].dynamic_id_ = Rec_.put_dynamic(
			ind_dynamic_par[i].value_
		);
		// i-th independent dynamic parameter has parameter index i + 1
		CPPAD_ASSERT_UNKNOWN(
			size_t( ind_dynamic_par[i].dynamic_id_ ) == i + 1
		);
	}

	// place each of the independent variables in the tape
	CPPAD_ASSERT_NARG_NRES(InvOp, 0, 1);
	for(size_t j = 0; j < n; j++)
	{	// tape address for this independent variable
		x[j].taddr_ = Rec_.PutOp(InvOp);
		x[j].tape_id_    = id_;
		CPPAD_ASSERT_UNKNOWN( size_t(x[j].taddr_) == j+1 );
		CPPAD_ASSERT_UNKNOWN( Variable(x[j] ) );
	}


	// done specifying all of the independent variables
	size_independent_ = n;
}
} } // END_CPPAD_LOCAL_NAMESPACE

# endif
