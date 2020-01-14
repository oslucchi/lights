#ifndef DEBUG_H
	#define DEBUG_H
	#include	<stdarg.h>
	extern	void	_DEBUG( const char *, ... );

	/*
	 * Conditional use of Debug.
	 * To use Debug directly, call _DEBUG.
	 *
	 * WARNING: in the source code, Debug calls MUST have double parentheses
	 *	    as:
	 *	    Debug(( 9, "Errno = %d\n", errno ));
	 *	    AND the first "(" MUST immediatly follow the "debug" word, thus:
	 *
	 *	    Debug(( 9, "Errno = %d\n", errno ));  -- CORRECT
	 *	    Debug (( 9, "Errno = %d\n", errno )); -- WRONG!!
	 *	    Debug( 9, "Errno = %d\n", errno );    -- WRONG!!
	 */

	#ifdef _DEBUG_
	# define Debug( stuff )			_DEBUG stuff
	#else
	# define Debug( stuff )			;
	#endif
#endif
