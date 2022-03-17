#include "ServiceManager.h"
//#include "../../../MContracts/src/Base.h"
//#include "../../../MContracts/src/VisualService/VisualService.h"
#include "Base.h"
#include "VisualService/VisualService.h"
#include <filesystem>

namespace AppSystem {

    AppSystem::Service_t ServiceManager::loadModule(std::string path) {
        AppSystem::Service_t mod;
        if (!std::filesystem::exists(path)) {
            //spdlog::error("{0} does not exist", path);
            mod.handle = NULL;
            return mod;
        }
        if (!std::filesystem::is_regular_file(path)) {
            //spdlog::error("{0} isn't a loadable module", path);
            mod.handle = NULL;
            return mod;
        }
#ifdef _WIN32
    mod.handle = LoadLibraryA(path.c_str());
    if (mod.handle == NULL) {
        //spdlog::error("Couldn't load {0}. Error code: {1}", path, GetLastError());
        mod.handle = NULL;
        return mod;
    }
    mod.info = (AppSystem::ServiceInfo_t*)GetProcAddress(mod.handle, "_INFO_");
    mod.init = (void (*)())GetProcAddress(mod.handle, "_INIT_");
    mod.createInstance = (AppSystem::Service * (*)(std::string)) GetProcAddress(mod.handle, "_CREATE_INSTANCE_");
    mod.deleteInstance = (void (*)(AppSystem::Service*))GetProcAddress(mod.handle, "_DELETE_INSTANCE_");
    mod.end = (void (*)())GetProcAddress(mod.handle, "_END_");
#else
    mod.handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (mod.handle == NULL) {
        //spdlog::error("Couldn't load {0}.", path);
        mod.handle = NULL;
        return mod;
    }
    mod.info = (AppSystem::ServiceInfo_t*)dlsym(mod.handle, "_INFO_");
    mod.init = (void (*)())dlsym(mod.handle, "_INIT_");
    mod.createInstance = (AppSystem::Service* (*)(std::string)) dlsym(mod.handle, "_CREATE_INSTANCE_");
    mod.deleteInstance = (void (*)(AppSystem::Service*))dlsym(mod.handle, "_DELETE_INSTANCE_");
    mod.end = (void (*)())dlsym(mod.handle, "_END_");
#endif
        if (mod.info == NULL) {
            //spdlog::error("{0} is missing _INFO_ symbol", path);
            mod.handle = NULL;
            return mod;
        }
        if (mod.init == NULL) {
            //spdlog::error("{0} is missing _INIT_ symbol", path);
            mod.handle = NULL;
            return mod;
        }
        if (mod.createInstance == NULL) {
            //spdlog::error("{0} is missing _CREATE_INSTANCE_ symbol", path);
            mod.handle = NULL;
            return mod;
        }
        if (mod.deleteInstance == NULL) {
            //spdlog::error("{0} is missing _DELETE_INSTANCE_ symbol", path);
            mod.handle = NULL;
            return mod;
        }
        if (mod.end == NULL) {
            //spdlog::error("{0} is missing _END_ symbol", path);
            mod.handle = NULL;
            return mod;
        }
        if (modules.find(mod.info->name) != modules.end()) {
            //spdlog::error("{0} has the same name as an already loaded module", path);
            mod.handle = NULL;
            return mod;
        }
        for (auto const& [name, _mod] : modules) {
            if (mod.handle == _mod.handle) {
                return _mod;
            }
        }
        mod.init();
        modules[mod.info->name] = mod;
        return mod;
    }

    int ServiceManager::createInstance(std::string name, std::string module) {
        if (modules.find(module) == modules.end()) {
            //spdlog::error("Module '{0}' doesn't exist", module);
            return -1;
        }
        if (instances.find(name) != instances.end()) {
            //spdlog::error("A module instance with the name '{0}' already exists", name);
            return -1;
        }
        int maxCount = modules[module].info->maxInstances;
        if (countModuleInstances(module) >= maxCount && maxCount > 0) {
            //spdlog::error("Maximum number of instances reached for '{0}'", module);
            return -1;
        }
        AppSystem::Instance_t inst;
        inst.module = modules[module];
        inst.instance = inst.module.createInstance(name);
        instances[name] = inst;
        //onInstanceCreated.emit(name);
        return 0;
    }

    int ServiceManager::deleteInstance(std::string name) {
        if (instances.find(name) == instances.end()) {
            //spdlog::error("Tried to remove non-existent instance '{0}'", name);
            return -1;
        }
        //onInstanceDelete.emit(name);
        AppSystem::Instance_t inst = instances[name];
        inst.module.deleteInstance(inst.instance);
        instances.erase(name);
        //onInstanceDeleted.emit(name);
        return 0;
    }

    int ServiceManager::deleteInstance(AppSystem::Service* instance) {
        //spdlog::error("Delete instance not implemented");
        return -1;
    }

    int ServiceManager::enableInstance(std::string name) {
        if (instances.find(name) == instances.end()) {
            //spdlog::error("Cannot enable '{0}', instance doesn't exist", name);
            return -1;
        }
        /*VisualService* v;
        v = dynamic_cast<VisualService*>(instances[name].instance);
        if (v==0) std::cout << "Null pointer on first type-cast" << std::endl;*/
        instances[name].instance->enable();
        return 0;
    }

    int ServiceManager::disableInstance(std::string name) {
        if (instances.find(name) == instances.end()) {
            //spdlog::error("Cannot disable '{0}', instance doesn't exist", name);
            return -1;
        }
        instances[name].instance->disable();
        return 0;
    }

    bool ServiceManager::instanceEnabled(std::string name) {
        if (instances.find(name) == instances.end()) {
            //spdlog::error("Cannot check if '{0}' is enabled, instance doesn't exist", name);
            return false;
        }
        return instances[name].instance->isEnabled();
    }

    void ServiceManager::postInit(std::string name) {
        if (instances.find(name) == instances.end()) {
            //spdlog::error("Cannot post-init '{0}', instance doesn't exist", name);
            return;
        }
        instances[name].instance->postInit();
    }

    std::string ServiceManager::getInstanceModuleName(std::string name) {
        if (instances.find(name) == instances.end()) {
            //spdlog::error("Cannot get module name of'{0}', instance doesn't exist", name);
            return "";
        }
        return std::string(instances[name].module.info->name);
    }

    int ServiceManager::countModuleInstances(std::string module) {
        if (modules.find(module) == modules.end()) {
            //spdlog::error("Cannot count instances of '{0}', Module doesn't exist", module);
            return -1;
        }
        AppSystem::Service_t mod = modules[module];
        int count = 0;
        for (auto const& [name, instance] : instances) {
            if (instance.module == mod) { count++; }
        }
        return count;
    }

    void ServiceManager::doPostInitAll() {
        for (auto& [name, inst] : instances) {
            //spdlog::info("Running post-init for {0}", name);
            inst.instance->postInit();
        }
    }
}