#pragma once
#include "Utils/Event.h"
#include <map>

#ifdef _WIN32
#include <Windows.h>
#endif

//#include "Module.h"
#include "../../../MContracts/src/Module.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "IOCContainer.h"
#include "ioc.h"
#include "Base.h"
namespace AppSystem
{

    class ServiceManager
    {
    private:
        /* data */
    public:
        spdlog::logger _logger;
        IOCContainer* _container;
        AppSystem::ioc::container* Container = new AppSystem::ioc::container();
        AppSystem::Service_t loadModule(std::string path);

        int createInstance(std::string name, std::string module);
        int deleteInstance(std::string name);
        int deleteInstance(AppSystem::Service *instance);

        int enableInstance(std::string name);
        int disableInstance(std::string name);
        bool instanceEnabled(std::string name);
        void postInit(std::string name);
        std::string getInstanceModuleName(std::string name);

        int countModuleInstances(std::string module);

        void doPostInitAll();

        Event<std::string> onInstanceCreated;
        Event<std::string> onInstanceDelete;
        Event<std::string> onInstanceDeleted;

        std::map<std::string, AppSystem::Service_t> modules;
        std::map<std::string, AppSystem::Instance_t> instances;
        spdlog::logger createLog();

        ServiceManager(/* args */);
        ~ServiceManager() = default;
    };

}