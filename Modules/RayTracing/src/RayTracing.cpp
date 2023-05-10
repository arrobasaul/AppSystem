#pragma once
#include "Base.h"
#include "imgui.h"
#include <cstdlib>  /* Aquí contiene la función system("pause"); */
#include <iostream> /* Importamos esta librería para hacer uso de cout y así ver el resultado en pantalla. */
#include <memory>
#include <string>
#include <functional>
#include <thread>

#include "ioc.h"
#include "MNetwork.h"
#include "VisualService/VisualService.h"
#include <irrKlang.h>
#include "ApplicationContext.h"
/*
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>

#include <opencv2/opencv.hpp>*/
using namespace irrklang;
//------------------------------------------------------------------------------
// using namespace cv;
// using namespace cv::dnn;

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

class RayTracing : public AppSystem::VisualService {
public:
  std::string paraPrueba = " paraPrueba RayTracing";
  std::string path = "Latino.mp4";
  
  RayTracing() {}
  RayTracing(std::string name) {
    this->name = name;
    std::string pathvid = "Latino.mp4";

    //if (!engine)
      //printf("error starting up the engine\n");
      //return 0; // error starting up the engine
    //else {
    // play some sound stream, looped
      //engine->play2D("getout.ogg", true);

      //char i = 0;
      //std::cin >> i; // wait for user to press some key

      // delete engine
    //}// gui::menu.registerEntry(name, menuHandler, this, NULL);
  }
  ISoundEngine* engine = createIrrKlangDevice();

  ~RayTracing() {
    // gui::menu.removeEntry(name);
    engine->drop(); 
  }
  
  void postInit() {}

  void enable() { enabled = true; }

  void disable() { enabled = false; }

  void OnAttach(std::shared_ptr<AppSystem::ApplicationContext> applicationContext) {}
  void OnDetach() {}
  void OnUpdate(float ts) {}
  bool isEnabled() { return enabled; }

  virtual void OnUIRender() override {
    // ImGui::Begin("Hello from RayTraicing");
    static char _host[128];
    static char _port[128];
    static char _target[128];
    static char _version[128];
    ImGui::InputText("Host 2", _host, IM_ARRAYSIZE(_host));
    ImGui::InputText("Port 2", _port, IM_ARRAYSIZE(_port));
    ImGui::InputText("Target 2", _target, IM_ARRAYSIZE(_target));
    ImGui::InputText("Version 2", _version, IM_ARRAYSIZE(_version));

    if (ImGui::Button("Save")) {
      auto const host = _host;
      auto const port = _port;
      auto const target = _target;
      /*int v = !std::strcmp("1.0", _version) ? 10 : 11;

      // The io_context is required for all I/O
      net::io_context ioc;
      auto start = std::chrono::system_clock::now();
      // Your Code to Execute //
      // Launch the asynchronous operation
      std::make_shared<session>(ioc)->run(host, port, target, v);
      auto end = std::chrono::system_clock::now();

      std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                         start)
                       .count()
                << "ms" << std::endl;
      // Run the I/O service. The call will return when
      // the get operation is complete.

      // std::thread thercontext = std::thread([&]()
      //                                  { ioc.run(); });
*/
    }
    // ImGui::End();
    /*ImGui::Begin("Hello video");
      Mat img;
      VideoCapture capturador(0);
      capturador.read(img);
      imshow("Image",img);
      waitKey(1);
    ImGui::End();*/
  }
  
private:
  static void menuHandler(void *ctx) {
    RayTracing *_this = (RayTracing *)ctx;
    // ImGui::Text("Hello SDR++, my name is %s", _this->name.c_str());
  }
  
  bool enabled = true;
  std::string name = "RayTracing";
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

MOD_EXPORT AppSystem::Service *_CREATE_INSTANCE_() {
  return new RayTracing();
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