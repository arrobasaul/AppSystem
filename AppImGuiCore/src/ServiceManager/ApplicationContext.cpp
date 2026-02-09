#include "ApplicationContext.h"

namespace AppSystem {
    ApplicationContext::ApplicationContext(std::shared_ptr<ServicePool> servicePool, std::shared_ptr<MessageBroker> messageBroker, void* context) 
    : servicePool(servicePool), messageBroker(messageBroker), context(context)
    { }
}