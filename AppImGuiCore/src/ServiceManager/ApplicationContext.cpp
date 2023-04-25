#include "ApplicationContext.h"

namespace AppSystem {
    ApplicationContext::ApplicationContext(std::shared_ptr<ServicePool> servicePool, std::shared_ptr<MessageBroker> messageBroker) 
    : servicePool(servicePool), messageBroker(messageBroker)
    { }
}