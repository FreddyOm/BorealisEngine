#pragma once

// Platform specific includes
#if defined(BOREALIS_WIN)

// @TODO: Differentiate rendering backends
#include "d3d11/d3d11.h"
#include "d3d12/d3d12.h"
#include "vulkan/vulkan.h"

#elif defined(BOREALIS_LINUX) || defined(BOREALIS_OSX)

#include "vulkan/vulkan.h"

#endif


#include "helpers/gbuffer.h"
