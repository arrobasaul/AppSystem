#include "Module.h"
#ifdef _WIN32
#ifdef APP_IS_CORE
#define APP_EXPORT extern "C" __declspec(dllexport)
#else
#define APP_EXPORT extern "C" __declspec(dllimport)
#endif
#else
#define APP_EXPORT extern
#endif

#ifdef _WIN32
#include <Windows.h>
#define MOD_EXPORT           extern "C" __declspec(dllexport)
#define APP_MOD_EXTENTSION ".dll"
#else
#include <dlfcn.h>
#define MOD_EXPORT extern "C"
#ifdef __APPLE__
#define APP_MOD_EXTENTSION ".dylib"
#else
#define APP_MOD_EXTENTSION ".so"
#endif
#endif

#define APP_MOD_INFO MOD_EXPORT const AppSystem::ServiceInfo_t _INFO_