#include "FirstApp.hpp"
#include "ServiceManager/ServiceManager.h"
#include <dirent.h>
#include <dlfcn.h>
#include <iostream>
// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class ExampleLayer : public AppSystem::Service {
public:
  
  bool enabled;

  virtual void OnUIRender() override {
    //ImGui::ShowDemoWindow();
  }
  
  void postInit() override {}

  void enable() override  { enabled = true; }

  void disable() override { enabled = false; }

  bool isEnabled() override { return enabled; }
private:
};

/*
int main() {

  AppEngine::FirstApp* app = new AppEngine::FirstApp();
  
  try {
    AppSystem::ServiceManager s;
    auto ins = s.instances;

    std::cout << ins.size() << std::endl;
    for (auto const &[key, val] : ins) {
      std::cout << "instances" << std::endl;
      std::cout << key << ':' << val.instance << std::endl;
      std::shared_ptr<AppSystem::Service> l(val.instance);
      app->PushLayer(l);
    }
    app->PushLayer<ExampleLayer>();
    app->SetMenubarCallback([app]() {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Exit")) {
          //app->Close();
        }
        ImGui::EndMenu();
      }
    });
    app->run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
*/