#include "ApplicationContext.h"

namespace AppSystem {
    ApplicationContext::ApplicationContext(std::shared_ptr<ServicePool> servicePool) 
    : servicePool(servicePool)
    { }
}