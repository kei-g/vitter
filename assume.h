#ifndef __include_assume_h__
#define __include_assume_h__

#ifndef __assume
	#define __assume(expr) __builtin_assume(expr)
#endif /* __assume */

#endif /* __include_assume_h__ */
