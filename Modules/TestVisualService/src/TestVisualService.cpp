#pragma once
#include "Base.h"
#include <imgui.h>
#include <cstdlib>  /* Aquí contiene la función system("pause"); */
#include <iostream> /* Importamos esta librería para hacer uso de cout y así ver el resultado en pantalla. */
#include <memory>
#include <string>
#include <functional>
#include <thread>

#include "ioc.h"
#include "MNetwork.h"
#include "VisualService/VisualService.h"
#include "ApplicationContext.h"
#include "MNetwork.h"
//#include "MNetwork.cpp"
//------------------------------------------------------------------------------

#ifndef _WIN32
#define EXPORT
#else
#define EXPORT __declspec(dllexport)
#endif
extern "C" EXPORT std::string Name() {
  char *nombre;
  std::string str_obj("TestVisualService");
  nombre = &str_obj[0];
  return str_obj;
}

APP_MOD_INFO{/* Name:            */ "TestVisualService",
             /* Description:     */ "My fancy new module",
             /* Author:          */ "arrobasaul",
             /* Version:         */ 0,
             1,
             0,
             /* Max instances    */ -1};

class TestVisualService : public AppSystem::VisualService {
public:
  TestVisualService(){}
  TestVisualService(std::string name) {
    this->name = name;
    
  }

  ~TestVisualService() {
    // gui::menu.removeEntry(name);
    //engine->drop(); 
  }
  
  void postInit() {}

  void enable() { enabled = true; }

  void disable() { enabled = false; }

  void OnAttach(std::shared_ptr<AppSystem::ApplicationContext> applicationContext) {
    //auto net = applicationContext->servicePool->getSingleService<IMNetwork>();

    //auto net2 = applicationContext->servicePool->getSingleServiceByName<Service>("MNetwork");

    //auto net22 = dynamic_pointer_cast<MNetwork>(net2);
    //std::cout << " esot es para " /*<< net22->paraPrueba */<< std::endl;
    //auto net3 = applicationContext->servicePool->getSingleServiceByName<VisualService>("RayTracing");
  
    //net->sayHello();
    ImGui::SetCurrentContext((ImGuiContext*)applicationContext->context);

  }
  void OnDetach() {}
  void OnUpdate(float ts) {}
  bool isEnabled() { return enabled; }

  virtual void OnUIRender() override {
    // ImGui::Begin("Hello from TestVisualService");
    static char _host[128];
    static char _port[128];
    static char _target[128];
    static char _version[128];
    ImGui::InputText("Host", _host, IM_ARRAYSIZE(_host));
    ImGui::InputText("Port", _port, IM_ARRAYSIZE(_port));
    ImGui::InputText("Target", _target, IM_ARRAYSIZE(_target));
    ImGui::InputText("Version", _version, IM_ARRAYSIZE(_version));

    if (ImGui::Button("Save")) {
      auto const host = _host;
      auto const port = _port;
      auto const target = _target;
    }
    // ImGui::End();
  }
  
private:
  static void menuHandler(void *ctx) {
    TestVisualService *_this = (TestVisualService *)ctx;
    // ImGui::Text("Hello SDR++, my name is %s", _this->name.c_str());
  }
  bool enabled = true;
  std::string name = "TestVisualService";
};

MOD_EXPORT void _INIT_(AppSystem::ioc::container* container) {

    //int retorno = gContainer->GetObject<IMNetwork>()->sayHello();
  // Resolve
    std::cout << "Resolving Interface" << std::endl;
    std::shared_ptr<IMNetwork> Value = container->resolve<IMNetwork>();
    int val = Value->sayHello();
    printf("You are in inside TestVisualService\n");
    printf("val " + val);
}

MOD_EXPORT AppSystem::Service *_CREATE_INSTANCE_() {
  return new TestVisualService();
}

MOD_EXPORT void _DELETE_INSTANCE_(void *instance) {
  delete (TestVisualService *)instance;
}

MOD_EXPORT void _END_() {
  // Nothing here
}
MOD_EXPORT AppSystem::Service *_CREATE_LAYER_(std::string name) {
  printf("_CREATE_LAYER_\n");
  return new TestVisualService(name);
}