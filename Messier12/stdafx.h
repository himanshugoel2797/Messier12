// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <wrl.h>

using namespace Microsoft::WRL;

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#include <d3d12.h>
#include <dxgi1_4.h>
#include "d3dx12.h"

#include <DirectXMath.h>

#pragma comment(lib, "dxgi")
#pragma comment(lib, "D3d12")
