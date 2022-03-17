#ifndef _MODULE_H_
#define _MODULE_H_

#include "Service.h"
#include <string>

namespace AppSystem { 
    
    struct ServiceInfo_t {
        const char* name;
        const char* description;
        const char* author;
        const int versionMajor;
        const int versionMinor;
        const int versionBuild;
        const int maxInstances;
    };
    struct Service_t {   
#ifdef _WIN32
        HMODULE handle;
#else
        void* handle;
#endif
        AppSystem::ServiceInfo_t* info;
        void (*init)();
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
    struct Instance_t {
        AppSystem::Service_t module;
        AppSystem::Service* instance;
    };
}
#endif