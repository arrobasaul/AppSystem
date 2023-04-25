#pragma once
#include "ServicePool.h"
#include "../include/MessageBroker.h"
namespace AppSystem
{

	class ApplicationContext
	{
	public:
        ApplicationContext()= default;
        ApplicationContext(std::shared_ptr<ServicePool> servicePool, std::shared_ptr<MessageBroker> messageBroker);

		virtual ~ApplicationContext() = default;
        std::shared_ptr<ServicePool> servicePool;
        std::shared_ptr<MessageBroker> messageBroker;
	};

}