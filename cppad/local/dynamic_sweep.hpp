# ifndef CPPAD_LOCAL_DYNAMIC_SWEEP_HPP
# define CPPAD_LOCAL_DYNAMIC_SWEEP_HPP

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-18 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

namespace CppAD { namespace local { // BEGIN_CPPAD_LOCAL_NAMESPACE
/*!
\file dynamic_sweep.hpp
Compute new value for dynamic parameters
*/

/*!
\def CPPAD_DYNAMIC_SWEEP_TRACE
This value is either zero or one.
Zero is the normal operational value.
If it is one, a trace of every forward0sweep computation is printed.
(Note that forward0sweep is not used if CPPAD_USE_DYANMIC_SWEEP is zero).
*/
# define CPPAD_DYNAMIC_SWEEP_TRACE 0

/*!
Compute dependent dynamic parammeters.


\param play
This play is const except for the parameter array.
The independcent dynamic parameters are inputs
and the dependent dynamic parameters are output
*/

template <class Base>
void dynamic_sweep(local::player<Base>* play)
{
	// parameters in player
	Base* parameter = play->parameter();

	// skip the BeginOp at the beginning of the recording
	size_t        i_op = 0;
	OpCode        op;
	const addr_t* arg;
	size_t        i_var;
	play->get_op_info(i_op, op, arg, i_var);
	CPPAD_ASSERT_UNKNOWN( op == BeginOp );

# if CPPAD_DYNAMIC_SWEEP_TRACE
	std::cout << std::endl;
# endif

	bool more_operators = true;
	while(more_operators)
	{	// op code for current instruction
		play->get_op_info(++i_op, op, arg, i_var);
		CPPAD_ASSERT_UNKNOWN( i_op < play->num_op_rec() );

		// action to take depends on the case
# if CPPAD_DYNAMIC_SWEEP_TRACE
		bool print = true;
# endif
		switch( op )
		{
			// -------------------------------------------------
			case MulppOp:
			CPPAD_ASSERT_NARG_NRES(op, 3, 0);
			parameter[arg[2]] = parameter[arg[0]] * parameter[arg[1]];
			break;

			// -------------------------------------------------
			case EndOp:
			CPPAD_ASSERT_NARG_NRES(op, 0, 0);
			more_operators = false;
			break;
			// -------------------------------------------------

			default:
# if CPPAD_DYNAMIC_SWEEP_TRACE
			print = false;
# endif
			break;
		}
# if CPPAD_DYNAMIC_SWEEP_TRACE
		if( print )
		{	printOp(
				std::cout,
				play,
				i_op,
				i_var,
				op,
				arg
			);
			std::cout << std::endl;
		}
# endif
	}
	return;
}

} } // END_CPPAD_LOCAL_NAMESPACE

// preprocessor symbols that are local to this file
# undef CPPAD_DYNAMIC_SWEEP_TRACE

# endif
