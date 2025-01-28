#pragma once

#define LYNXSCRIPT_VERSION_MAJOR 0
#define LYNXSCRIPT_VERSION_MINOR 1
#define LYNXSCRIPT_VERSION_PATCH 0

#define LYNXSCRIPT_VERSION_STRING "0.1.0"

#define LYNXSCRIPT_VERSION (LYNXSCRIPT_VERSION_MAJOR * 10000 + LYNXSCRIPT_VERSION_MINOR * 100 + LYNXSCRIPT_VERSION_PATCH)

#ifndef LYNX_API
	#if defined(_MSC_VER) && defined(LYNX_API_DLLEXPORT)
		#define LYNX_API __declspec(dllexport)
	#else
		#define LYNX_API
	#endif
#endif

typedef void (*LynxErrorFn)(int errorCode);

LYNX_API void lxSetErrorCallback(LynxErrorFn callback);
LYNX_API void lxInitVM();
LYNX_API void lxExecuteScript(const char* source);
LYNX_API void lxExecuteFile(const char* path);
LYNX_API void lxFreeVM();