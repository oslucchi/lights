#ifndef DEBUG_H
	#define DEBUG_H
	#include	<stdarg.h>
	extern	void	_DEBUG( int, ... );

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
		#ifdef _DEBUGARDCTRL_
			#define DebugARDCtrl( stuff )			_DEBUG stuff
		#else
			#define DebugARDCtrl( stuff )			;
		#endif
		#ifdef _DEBUGRGB_
			#define DebugRGB( stuff )			_DEBUG stuff
		#else
			#define DebugRGB( stuff )			;
		#endif
		#ifdef _DEBUGI2CCOMM_
			# define DebugI2CComm( stuff )		_DEBUG stuff
		#else
			#define DebugI2CComm( stuff )		;
		#endif
		#ifdef _DEBUGTMRMNG_
			# define DebugTMRMng( stuff )		_DEBUG stuff
		#else
			#define DebugTMRMng( stuff )		;
		#endif
	#else
		#define DebugARDCtrl( stuff )			;
		#define DebugRGB( stuff )			;
		#define DebugI2CComm( stuff )		;
		#define DebugTMRMng( stuff )		;
	#endif
#endif
