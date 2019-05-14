#include "assert.h"

#ifndef __MUHKUH_STATIC_ASSERT_H__
#define __MUHKUH_STATIC_ASSERT_H__

#if defined(__cpp_static_assert)
#       define MUHKUH_STATIC_ASSERT(CONDITION, MESSAGE) static_assert(CONDITION, MESSAGE)
#else
#       define MUHKUH_STATIC_ASSERT(CONDITION, MESSAGE)
#endif

#endif  /* __MUHKUH_STATIC_ASSERT_H__ */
