#pragma once
#include "ServicePool.h"
namespace AppSystem
{

	class ApplicationContext
	{
	public:
        ApplicationContext()= default;
        ApplicationContext(std::shared_ptr<ServicePool> servicePool);
		virtual ~ApplicationContext() = default;
        std::shared_ptr<ServicePool> servicePool;
	};

}