#pragma once
#include "Base.h"
#include "Layer.h"
#include "imgui.h"
#include <cstdlib>  /* Aquí contiene la función system("pause"); */
#include <iostream> /* Importamos esta librería para hacer uso de cout y así ver el resultado en pantalla. */
#include <memory>
#include <string>
#include <functional>
#include <thread>

#include "ioc.h"
#include "MNetwork.h"
#include <irrKlang.h>
using namespace irrklang;
//------------------------------------------------------------------------------

#ifndef _WIN32
#define EXPORT
#else
#define EXPORT __declspec(dllexport)
#endif
extern "C" EXPORT std::string Name() {
  char *nombre;
  std::string str_obj("RayTracing");
  nombre = &str_obj[0];
  return str_obj;
}

APP_MOD_INFO{/* Name:            */ "RayTracing",
             /* Description:     */ "My fancy new module",
             /* Author:          */ "arrobasaul",
             /* Version:         */ 0,
             1,
             0,
             /* Max instances    */ -1};

class RayTracing : public AppSystem::Service {
public:
  
  RayTracing(std::string name) {
    this->name = name;
    

    if (!engine)
      printf("error starting up the engine\n");
      //return 0; // error starting up the engine
    else {
    // play some sound stream, looped
      engine->play2D("getout.ogg", true);

      char i = 0;
      //std::cin >> i; // wait for user to press some key

      // delete engine
    }// gui::menu.registerEntry(name, menuHandler, this, NULL);
  }
  ISoundEngine* engine = createIrrKlangDevice();

  ~RayTracing() {
    // gui::menu.removeEntry(name);
    engine->drop(); 
  }
  
  void postInit() {}

  void enable() { enabled = true; }

  void disable() { enabled = false; }

  bool isEnabled() { return enabled; }
  virtual void OnUIRender() override {
    
  }
  
private:
  static void menuHandler(void *ctx) {
    RayTracing *_this = (RayTracing *)ctx;
    // ImGui::Text("Hello SDR++, my name is %s", _this->name.c_str());
  }
  bool enabled = true;
  std::string name;
};

MOD_EXPORT void _INIT_(AppSystem::ioc::container* container) {

    //int retorno = gContainer->GetObject<IMNetwork>()->sayHello();
  // Resolve
    std::cout << "Resolving Interface" << std::endl;
    std::shared_ptr<IMNetwork> Value = container->resolve<IMNetwork>();
    int val = Value->sayHello();
    printf("You are in inside Mcore2 on win fron other \n");
    printf("val " + val);
}

MOD_EXPORT AppSystem::Service *_CREATE_INSTANCE_(std::string name) {
  return new RayTracing(name);
}

MOD_EXPORT void _DELETE_INSTANCE_(void *instance) {
  delete (RayTracing *)instance;
}

MOD_EXPORT void _END_() {
  // Nothing here
}
MOD_EXPORT AppSystem::Service *_CREATE_LAYER_(std::string name) {
  printf("_CREATE_LAYER_\n");
  return new RayTracing(name);
}