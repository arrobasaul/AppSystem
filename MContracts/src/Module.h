#ifndef _MODULE_H_
#define _MODULE_H_

#include "Service.h"
#include "ioc.h"

#include <string>
#ifdef _WIN32
    #include <Windows.h>
    #ifdef _WINDLL
    #define DLL_PORT __declspec(dllexport)
    #else
    #define DLL_PORT __declspec(dllimport)
#endif
#else
    #define DLL_PORT
#endif


namespace AppSystem { 
    
    struct DLL_PORT ServiceInfo_t {
        const char* name;
        const char* description;
        const char* author;
        const int versionMajor;
        const int versionMinor;
        const int versionBuild;
        const int maxInstances;
    };
    struct DLL_PORT Service_t {
#ifdef _WIN32
        HMODULE handle;
#else
        void* handle;
#endif
        AppSystem::ServiceInfo_t* info;
        void (*init)( AppSystem::ioc::container* container );
        AppSystem::Service* (*createInstance)(std::string name);
        void (*deleteInstance)(AppSystem::Service* instance);
        void (*end)();

        friend bool operator==(const Service_t& a, const Service_t& b) {
            if (a.handle != b.handle) { return false; }
            if (a.info != b.info) { return false; }
            if (a.init != b.init) { return false; }
            if (a.createInstance != b.createInstance) { return false; }
            if (a.deleteInstance != b.deleteInstance) { return false; }
            if (a.end != b.end) { return false; }
            return true;
        }
    };
    struct DLL_PORT Instance_t {
        AppSystem::Service_t module;
        AppSystem::Service* instance;
    };
}
#endif