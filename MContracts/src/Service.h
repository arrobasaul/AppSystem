#ifndef _SERVICE_H_
#define _SERVICE_H_

namespace AppSystem
{
    class Service
    {
    private:
        /* data */
    public:
        // virtual void onInit() = 0;
        virtual void postInit() = 0;
        virtual void enable() = 0;
        virtual void disable() = 0;
        virtual bool isEnabled() = 0;
        virtual ~Service() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}

        virtual void OnUpdate(float ts) {}
        virtual void OnUIRender() {}
    };
}
#endif
