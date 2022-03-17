#ifndef _VISUALSERVICE_H_
#define _VISUALSERVICE_H_
#include "../Service.h"
namespace AppSystem {
    class VisualService: public Service
    {
    private:
        /* data */
    public:
        virtual void OnGuiRender() = 0;
    };
}

#endif