#pragma once
#include <memory>
#include <vector>
#include "ioc.h"
namespace AppSystem
{

	class ServicePool
	{
    private:
        AppSystem::ioc::container* ioc;
	public:
        ServicePool() = default;
        ServicePool(AppSystem::ioc::container* container)
        :ioc(container){
            
        }
		virtual ~ServicePool() = default;
        template<typename S> 
            std::shared_ptr< S> getSingleService(){
                return ioc->resolve<S>();
            }
        template<typename S> 
            std::shared_ptr<S> getSingleServiceByName(std::string name){
                const std::string registration_name = name;
                return ioc->resolve_by_name<S>(registration_name);
            }
        template<typename S> 
            std::vector<std::shared_ptr<S>> getServices(){
                return ioc->resolve_by_type<S>();
            }
	};

}