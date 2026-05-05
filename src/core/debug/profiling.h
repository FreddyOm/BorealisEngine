#pragma once

//#define USE_PIX_PROFILER

#ifdef USE_PIX_PROFILER

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
#include <pix3.h>
#endif

#endif