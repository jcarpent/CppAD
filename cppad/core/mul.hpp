# ifndef CPPAD_CORE_MUL_HPP
# define CPPAD_CORE_MUL_HPP

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-18 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

//  BEGIN CppAD namespace
namespace CppAD {

template <class Base>
AD<Base> operator * (const AD<Base> &left , const AD<Base> &right)
{
	// compute the Base part
	AD<Base> result;
	result.value_  = left.value_ * right.value_;
	CPPAD_ASSERT_UNKNOWN( Parameter(result) );

	// check if there is a recording in progress
	local::ADTape<Base>* tape = AD<Base>::tape_ptr();
	if( tape == CPPAD_NULL )
		return result;
	tape_id_t tape_id = tape->id_;

	// tape_id cannot match the default value for tape_id_; i.e., 0
	CPPAD_ASSERT_UNKNOWN( tape_id > 0 );
	bool var_left  = (left.tape_id_  == tape_id) & (left.taddr_  != 0);
	bool var_right = (right.tape_id_ == tape_id) & (right.taddr_ != 0);

	if( var_left )
	{	if( var_right )
		{	// result = variable * variable
			CPPAD_ASSERT_UNKNOWN( local::NumRes(local::MulvvOp) == 1 );
			CPPAD_ASSERT_UNKNOWN( local::NumArg(local::MulvvOp) == 2 );

			// put operand addresses in tape
			tape->Rec_.PutArg(left.taddr_, right.taddr_);
			// put operator in the tape
			result.taddr_ = tape->Rec_.PutOp(local::MulvvOp);
			// make result a variable
			result.tape_id_ = tape_id;
		}
		else if( IdenticalZero(right.value_) )
		{	// result = variable * 0
		}
		else if( IdenticalOne(right.value_) )
		{	// result = variable * 1
			result.make_variable(left.tape_id_, left.taddr_);
		}
		else
		{	// result = variable * parameter
			CPPAD_ASSERT_UNKNOWN( local::NumRes(local::MulpvOp) == 1 );
			CPPAD_ASSERT_UNKNOWN( local::NumArg(local::MulpvOp) == 2 );

			// put operand addresses in tape
			addr_t p = right.dynamic_id_;
			if( p == 0 )
				p = tape->Rec_.PutPar(right.value_);
			tape->Rec_.PutArg(p, left.taddr_);

			// put operator in the tape and make result a variable
			result.taddr_   = tape->Rec_.PutOp(local::MulpvOp);
			result.tape_id_ = tape_id;
		}
	}
	else if( var_right )
	{	if( IdenticalZero(left.value_) )
		{	// result = 0 * variable
		}
		else if( IdenticalOne(left.value_) )
		{	// result = 1 * variable
			result.make_variable(right.tape_id_, right.taddr_);
		}
		else
		{	// result = parameter * variable
			CPPAD_ASSERT_UNKNOWN( local::NumRes(local::MulpvOp) == 1 );
			CPPAD_ASSERT_UNKNOWN( local::NumArg(local::MulpvOp) == 2 );

			// put operand addresses in tape
			addr_t p = left.dynamic_id_;
			if( p == 0 )
				p = tape->Rec_.PutPar(left.value_);
			tape->Rec_.PutArg(p, right.taddr_);

			// put operator in the tape and make result a variable
			result.taddr_   = tape->Rec_.PutOp(local::MulpvOp);
			result.tape_id_ = tape_id;
		}
	}
	else
	{	bool dyn_left  = (left.tape_id_  == tape_id) & (left.dynamic_id_ != 0);
		bool dyn_right = (right.tape_id_ == tape_id) & (right.dynamic_id_ != 0);
		if( dyn_left | dyn_right )
		{	// dynamic parameter = parameter * parameter
			CPPAD_ASSERT_NARG_NRES(local::MulppOp, 3, 0);
			//
			// left parameter
			addr_t p = left.dynamic_id_;
			if( p == 0 )
				p = tape->Rec_.PutPar(left.value_);
			tape->Rec_.PutArg(p);
			//
			// right parameter
			p = right.dynamic_id_;
			if( p == 0 )
				p = tape->Rec_.PutPar(right.value_);
			tape->Rec_.PutArg(p);
			//
			// result parameter
			p = tape->Rec_.put_dynamic(result.value_);
			CPPAD_ASSERT_UNKNOWN( p != 0 );
			tape->Rec_.PutArg(p);
			//
			// put operator in the tape
			tape->Rec_.PutOp(local::MulppOp);
			//
			// make the result a dynamic parmaeter
			result.dynamic_id_ = p;
			result.tape_id_    = tape_id;
		}
	}
	return result;
}

// convert other cases into the case above
CPPAD_FOLD_AD_VALUED_BINARY_OPERATOR(*)

} // END CppAD namespace

# endif
