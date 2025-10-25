#pragma once

// Definitions of use of Win headers!

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

// D3D11 specific headers

#include <d3d11.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgiformat.h>
// #include <d3dcommon.h>

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
#ifndef D3DCOMPILE_DEBUG
	#define D3DCOMPILE_DEBUG
#endif
#include <d3d11sdklayers.h>	// Debug Layer
#include <dxgidebug.h>
#endif

// Borealis API-specific includes
// #include "../helpers/d3d11_helpers.h" // <-- Not yet implemented

// Borealis API-agnostic helpers
#include "../helpers/helpers.h"