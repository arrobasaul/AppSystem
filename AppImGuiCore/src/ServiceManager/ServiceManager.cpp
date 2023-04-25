#include "ServiceManager.h"
//#include "../../../MContracts/src/Base.h"
//#include "../../../MContracts/src/VisualService/VisualService.h"
#include "Base.h"
#include "VisualService/VisualService.h"
#include <filesystem>
#include <iostream>

namespace AppSystem
{
    static void PrintException( const char *Function, const std::exception &e )
{
    std::cout << "Exception in " 
        << Function << ", " 
        << e.what() << std::endl;
}
class IAmAThing
{
public:
   virtual ~IAmAThing() { }
   virtual void TestThis() = 0;
};

class TheThing: public IAmAThing
{
public:
   TheThing()
   {
   }
   void TestThis()
   {
      std::cout << "A Thing from TheThing" << std::endl;
   }
};
    AppSystem::Service_t ServiceManager::loadModule(std::string path)
    {
        std::cout << "loadModule" << std::endl;
        AppSystem::Service_t mod;
        if (!std::filesystem::exists(path))
        {
            _logger.error("{0} does not exist", path);
            mod.handle = NULL;
            return mod;
        }
        if (!std::filesystem::is_regular_file(path))
        {
            _logger.error("{0} isn't a loadable module", path);
            mod.handle = NULL;
            return mod;
        }
#ifdef _WIN32
        mod.handle = LoadLibraryA(path.c_str());
        if (mod.handle == NULL)
        {
            _loggererror("Couldn't load {0}. Error code: {1}", path, GetLastError());
            mod.handle = NULL;
            return mod;
        }
        mod.info = (AppSystem::ServiceInfo_t *)GetProcAddress(mod.handle, "_INFO_");
        mod.init = (void (*)())GetProcAddress(mod.handle, "_INIT_");
        mod.createInstance = (AppSystem::Service * (*)(std::string)) GetProcAddress(mod.handle, "_CREATE_INSTANCE_");
        mod.deleteInstance = (void (*)(AppSystem::Service *))GetProcAddress(mod.handle, "_DELETE_INSTANCE_");
        mod.end = (void (*)())GetProcAddress(mod.handle, "_END_");
#else
        mod.handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (mod.handle == NULL)
        {
            _logger.error("Couldn't load {0}.", path);
        std::cout << "Couldn't load " << path << std::endl;

            mod.handle = NULL;
            return mod;
        }
        
        mod.info = (AppSystem::ServiceInfo_t *)dlsym(mod.handle, "_INFO_");
        mod.init = (void (*)(AppSystem::ioc::container* container))dlsym(mod.handle, "_INIT_");
        mod.createInstance = (AppSystem::Service * (*)()) dlsym(mod.handle, "_CREATE_INSTANCE_");
        mod.deleteInstance = (void (*)(AppSystem::Service *))dlsym(mod.handle, "_DELETE_INSTANCE_");
        mod.end = (void (*)())dlsym(mod.handle, "_END_");
#endif
        if (mod.info == NULL)
        {
            _logger.error("{0} is missing _INFO_ symbol", path);
            mod.handle = NULL;
            return mod;
        }
        if (mod.init == NULL)
        {
            _logger.error("{0} is missing _INIT_ symbol", path);
            mod.handle = NULL;
            return mod;
        }
        if (mod.createInstance == NULL)
        {
            _logger.error("{0} is missing _CREATE_INSTANCE_ symbol", path);
            mod.handle = NULL;
            return mod;
        }
        if (mod.deleteInstance == NULL)
        {
            _logger.error("{0} is missing _DELETE_INSTANCE_ symbol", path);
            mod.handle = NULL;
            return mod;
        }
        if (mod.end == NULL)
        {
            _logger.error("{0} is missing _END_ symbol", path);
            mod.handle = NULL;
            return mod;
        }
        if (modules.find(mod.info->name) != modules.end())
        {
            _logger.error("{0} has the same name as an already loaded module", path);
            mod.handle = NULL;
            return mod;
        }
        for (auto const &[name, _mod] : modules)
        {
            if (mod.handle == _mod.handle)
            {
                return _mod;
            }
        }
        modules[mod.info->name] = mod;
        createInstance(mod.info->name, mod.info->name);
        mod.init(Container);
        return mod;
    }

    int ServiceManager::createInstance(std::string name, std::string module)
    {
        if (modules.find(module) == modules.end())
        {
            _logger.error("Module '{0}' doesn't exist", module);
            return -1;
        }
        if (instances.find(name) != instances.end())
        {
            _logger.error("A module instance with the name '{0}' already exists", name);
            return -1;
        }
        int maxCount = modules[module].info->maxInstances;
        if (countModuleInstances(module) >= maxCount && maxCount > 0)
        {
            _logger.error("Maximum number of instances reached for '{0}'", module);
            return -1;
        }
        AppSystem::Instance_t inst;
        inst.module = modules[module];
        
        inst.instance = inst.module.createInstance();
        
        if (VisualService* c  = dynamic_cast<VisualService*>(inst.instance) )
        {
            instances[name] = inst;
            std::shared_ptr<VisualService> sr(dynamic_cast<VisualService*>(inst.instance));
            //Container->register_delegate_with_name<VisualService>(name, inst.module.createInstance);
            Container->register_instance_with_name<VisualService>(name, sr);
        }
        else{
            instances[name] = inst;
            // Container->register_instance_with_name<Service>(name, sr);
            Container->register_delegate_with_name<Service>(name, inst.module.createInstance);

        }
        // onInstanceCreated.emit(name);
        return 0;
    }

    int ServiceManager::deleteInstance(std::string name)
    {
        if (instances.find(name) == instances.end())
        {
            _logger.error("Tried to remove non-existent instance '{0}'", name);
            return -1;
        }
        // onInstanceDelete.emit(name);
        AppSystem::Instance_t inst = instances[name];
        inst.module.deleteInstance(inst.instance);
        instances.erase(name);
        // onInstanceDeleted.emit(name);
        return 0;
    }

    int ServiceManager::deleteInstance(AppSystem::Service *instance)
    {
        _logger.error("Delete instance not implemented");
        return -1;
    }

    int ServiceManager::enableInstance(std::string name)
    {
        if (instances.find(name) == instances.end())
        {
            _logger.error("Cannot enable '{0}', instance doesn't exist", name);
            return -1;
        }
        /*VisualService* v;
        v = dynamic_cast<VisualService*>(instances[name].instance);
        if (v==0) std::cout << "Null pointer on first type-cast" << std::endl;*/
        if (VisualService* c = dynamic_cast<VisualService*>(instances[name].instance))
        {
            // do Child specific stuff
            //instances[name].instance->enable();
            c->enable();
        }
        return 0;
    }

    int ServiceManager::disableInstance(std::string name)
    {
        if (instances.find(name) == instances.end())
        {
            _logger.error("Cannot disable '{0}', instance doesn't exist", name);
            return -1;
        }
        if (VisualService* c = dynamic_cast<VisualService*>(instances[name].instance))
        {
            // do Child specific stuff
            //instances[name].instance->enable();
            c->disable();
        }
        return 0;
    }

    bool ServiceManager::instanceEnabled(std::string name)
    {
        if (instances.find(name) == instances.end())
        {
            _logger.error("Cannot check if '{0}' is enabled, instance doesn't exist", name);
            return false;
        }
        if (VisualService* c = dynamic_cast<VisualService*>(instances[name].instance))
        {
            // do Child specific stuff
            //instances[name].instance->enable();
            c->isEnabled();
        }
        return false;
    }

    void ServiceManager::postInit(std::string name)
    {
        if (instances.find(name) == instances.end())
        {
            _logger.error("Cannot post-init '{0}', instance doesn't exist", name);
            return;
        }
        if (VisualService* c = dynamic_cast<VisualService*>(instances[name].instance))
        {
            // do Child specific stuff
            //instances[name].instance->enable();
            c->postInit();
        }
        //instances[name].instance->postInit();
    }

    std::string ServiceManager::getInstanceModuleName(std::string name)
    {
        if (instances.find(name) == instances.end())
        {
            _logger.error("Cannot get module name of'{0}', instance doesn't exist", name);
            return "";
        }
        return std::string(instances[name].module.info->name);
    }

    int ServiceManager::countModuleInstances(std::string module)
    {
        if (modules.find(module) == modules.end())
        {
            _logger.error("Cannot count instances of '{0}', Module doesn't exist", module);
            return -1;
        }
        AppSystem::Service_t mod = modules[module];
        int count = 0;
        for (auto const &[name, instance] : instances)
        {
            if (instance.module == mod)
            {
                count++;
            }
        }
        return count;
    }

    void ServiceManager::doPostInitAll()
    {
        for (auto &[name, inst] : instances)
        {
            _logger.info("Running post-init for {0}", name);
             if (VisualService* c = dynamic_cast<VisualService*>(instances[name].instance))
            {
                // do Child specific stuff
                //instances[name].instance->enable();
                c->postInit();
            }
            //inst.instance->postInit();
        }
    }
    ServiceManager::ServiceManager()
    : _logger(createLog()), _container()
    {
        //_container->RegisterInstance<IAmAThing,TheThing>();

        try
        {
            // Register
            std::cout << "Registering Concretion as Interface" << std::endl;
            Container->register_type<IAmAThing, TheThing>();
            // Resolve
            std::cout << "Resolving Interface" << std::endl;
            std::shared_ptr<IAmAThing> Value = Container->resolve<IAmAThing>();
            if( Value.get() )
            {
                std::cout << "Successfully resolved Interface" << std::endl;
            }
        }
        catch( const std::exception &e )
        {
            PrintException( __func__, e );
        }
        std::string path = "modules";
        std::cout << path << std::endl;
        for (const auto &file : std::filesystem::recursive_directory_iterator(path))
        {
            auto nFile = file.path().string();
            std::cout << nFile << std::endl;
            if (nFile.find(APP_MOD_EXTENTSION) != std::string::npos)
            {
                loadModule(nFile);
                std::cout << file.path() << std::endl;
            }
        }
        Container->resolve_by_type<Service>();

        servicePool = std::make_shared<ServicePool>(Container);
        messageBroker = std::make_shared<MessageBroker>(_bus);
        applicationContext = std::make_shared<ApplicationContext>(servicePool,messageBroker);
        /*for(auto valor : Container->resolve_by_type<Service>()){
            visual.push_back(std::shared_ptr<VisualService>(reinterpret_cast<VisualService *>( valor->create_item())));
        }*/
    }
    spdlog::logger ServiceManager::createLog(){
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::warn);
        console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
        file_sink->set_level(spdlog::level::trace);

        spdlog::logger _logger = spdlog::logger("multi_sink", {console_sink, file_sink});
        _logger.set_level(spdlog::level::debug);
        _logger.warn("this should appear in both console and file");
        _logger.info("this message should not appear in the console, only in the file");
        return _logger;
    }

}