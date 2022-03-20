#include <cstdlib> /* Aquí contiene la función system("pause"); */
#include <iostream> /* Importamos esta librería para hacer uso de cout y así ver el resultado en pantalla. */
#include <string>
#include "Base.h"

#ifndef _WIN32
    #define EXPORT
    #else
    #define EXPORT __declspec(dllexport) 
#endif
extern "C" EXPORT std::string Name() {
    char* nombre;
    std::string str_obj("GeeksForGeeks");
    nombre = &str_obj[0];
    return str_obj;
} 


APP_MOD_INFO{
    /* Name:            */ "Mcore2",
    /* Description:     */ "My fancy new module",
    /* Author:          */ "author1;author2,author3,etc...",
    /* Version:         */ 0, 1, 0,
    /* Max instances    */ -1
};

class MCore : public AppSystem::Service {
public:
    MCore(std::string name) {
        this->name = name;
        //gui::menu.registerEntry(name, menuHandler, this, NULL);
    }

    ~MCore() {
        //gui::menu.removeEntry(name);
    }

    void postInit() {
       
    }

    void enable() {
        enabled = true;
    }

    void disable() {
        enabled = false;
    }

    bool isEnabled() {
        return enabled;
    }

private:
    static void menuHandler(void* ctx) {
        MCore* _this = (MCore*)ctx;
        //ImGui::Text("Hello SDR++, my name is %s", _this->name.c_str());
    }

    std::string name;
    bool enabled = true;
};

MOD_EXPORT void _INIT_() {
    // Nothing here
    printf("You are in inside Mcore2 on win\n");
}

MOD_EXPORT AppSystem::Service* _CREATE_INSTANCE_(std::string name) {
    return new MCore(name);
}

MOD_EXPORT void _DELETE_INSTANCE_(void* instance) {
    delete (MCore*)instance;
}

MOD_EXPORT void _END_() {
    // Nothing here
}