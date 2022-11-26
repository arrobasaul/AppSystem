#pragma once
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

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}
template<typename Child, typename T>
inline bool childof(const T *ptr) {
    return dynamic_cast<const Child*>(ptr) != nullptr;
}

#define APP_MOD_INFO MOD_EXPORT const AppSystem::ServiceInfo_t _INFO_