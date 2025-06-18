#pragma once

// Platform specific includes
#if defined(BOREALIS_WIN)

// TODO: Differentiate rendering backends
// TODO: Also create common interface and bnackend independence layer
// TODO: Consider using d3d11on12 https://github.com/microsoft/D3D11On12/tree/master

#include "d3d11/borealis_d3d11.h"
#include "d3d12/borealis_d3d12.h"
#include "vulkan/borealis_vulkan.h"

#elif defined(BOREALIS_LINUX) || defined(BOREALIS_OSX)
// TODO: Implement
#include "vulkan/borealis_vulkan.h"

#endif

#include "helpers/helpers.h"

