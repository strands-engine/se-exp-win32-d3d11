#pragma once

// Header meant to be included on top of every win32 dependent file. It wraps inclusion
//  of <Windows.h> header by providing handful of useful defines that limit code size and
//  unnecessary namespace clutter.
// Inspired and based by a solution given in Chili Direct3D Engine by PlanetChili (see:
//  http://www.planetchili.net for more info.

// indicates which minimal version of Windows is tagreted
//  - by value given below: Windows 7 or later
#define _WIN32_WINNT 0x0601

// applies default version to win-specific macros based on targeted version
#include <sdkddkver.h>

// strips unnecessary clutter from included <Windows.h>
#define WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE

#define NOMINMAX

#define STRICT

#include <Windows.h>

#include <wrl/client.h>
namespace d3dexp::chili
{
	template <typename T>
	using com_ptr = Microsoft::WRL::ComPtr<T>;

	template <typename T>
	void** to_pp(com_ptr<T>& p)
	{
		return reinterpret_cast<void**>(p.GetAddressOf());
	}
}