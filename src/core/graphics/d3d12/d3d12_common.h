#pragma once

// Definitions for use of Win headers!
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
	#define NOMINMAX
#endif

// Windows and COM specific headers
#include <Windows.h>
#include <wrl.h>

// D3D12 specific headers
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgiformat.h>
//#include <d3dcommon.h>

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
#define D3DCOMPILE_DEBUG
#include <d3d12sdklayers.h>	// Debug Layer
#include <dxgidebug.h>
#endif

// Borealis API-specific includes
//#include "../helpers/d3d12_helpers.h" // <-- Not yet implemented

// Borealis API-agnostic helpers
#include "../helpers/helpers.h"
