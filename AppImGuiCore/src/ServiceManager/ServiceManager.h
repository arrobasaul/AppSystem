//#include "Module.h"
#include "../Utils/Event.h"
#include "Module.h"
#include <map>

namespace AppSystem {

    class ServiceManager
    {
    private:
        /* data */
    public:
        //AppSystem::Service_t loadModule(std::string path);

        int createInstance(std::string name, std::string module);
        int deleteInstance(std::string name);
        int deleteInstance(AppSystem::Service* instance);

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
    };
    
}