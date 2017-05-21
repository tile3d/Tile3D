#pragma once

#include <assert.h>

#ifdef DEBUG_VERSION
#define TAssert assert
#else
#define TAssert 
#endif

