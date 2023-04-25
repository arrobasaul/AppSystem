#ifndef _VISUALSERVICE_H_
#define _VISUALSERVICE_H_
#include "../Service.h"
#include "ApplicationContext.h"
namespace AppSystem {
    class VisualService: public Service
    {
    private:
        /* data */
    public:
        virtual void postInit() = 0;
        virtual void enable() = 0;
        virtual void disable() = 0;
        virtual bool isEnabled() = 0;
        virtual void OnUpdate(float ts) = 0;
        virtual void OnUIRender() = 0;
    };
}

#endif