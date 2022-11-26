#ifndef _SERVICE_H_
#define _SERVICE_H_
#include <memory>
#include "ApplicationContext.h"
namespace AppSystem
{
    class Service
    {
    private:
        /* data */
    public:
        // virtual void onInit() = 0;
        
        virtual ~Service() = default;

        virtual void OnAttach(std::shared_ptr<ApplicationContext> applicationContext) = 0;
        virtual void OnDetach() = 0;

        //virtual void OnUpdate(float ts) {}
        //virtual void OnUIRender() {}
    };
}
#endif
