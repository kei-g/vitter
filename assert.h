#ifndef __include_assert_h__
#define __include_assert_h__

#ifdef _DEBUG
	#include <assert.h>
	#define ASSERT(expr) assert(expr)
#else /* _DEBUG */
	#define ASSERT(expr)
#endif /* _DEBUG */

#endif /* __include_assert_h__ */
