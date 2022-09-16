#include "Application.h"
//#include "Walnut/EntryPoint.h"
#include "Image.h"
#include "ServiceManager/ServiceManager.h"
#include <dirent.h>
#include <dlfcn.h>
#include <iostream>

//#include "Config.h"

bool g_ApplicationRunning = true;

class ExampleLayer : public AppSystem::Service {
public:
  bool enabled;
  virtual void OnUIRender() override {
    ImGui::Begin("Hello");
    ImGui::Button("Button");
    ImGui::End();

    ImGui::ShowDemoWindow();
  }
  void postInit() {}

  void enable() { enabled = true; }

  void disable() { enabled = false; }

  bool isEnabled() { return enabled; }
};
struct Moder {
  void *handle;
  AppSystem::Service *(*createInstance)(std::string name);
};
AppSystem::Application *AppSystem::CreateApplication(int argc, char **argv) {

  std::cout << "aqui" << std::endl;
  AppSystem::ApplicationSpecification spec;
  spec.Name = "AppSystem Example";

  AppSystem::Application *app = new AppSystem::Application(spec);
  /*Moder layers;
  std::string path = "modules/libMNetwork.so";
  layers.handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
  std::cout << "open" << std::endl;
  std::cout << layers.handle << std::endl;
  layers.createInstance = (AppSystem::Layer * (*)(std::string))
  dlsym(layers.handle, "_CREATE_LAYER_"); std::cout << "create" << std::endl;
  std::cout << *layers.createInstance << std::endl;
  auto lay = layers.createInstance("Network");*/
  // std::shared_ptr<Layer> l(lay);
  //  app->PushLayer(l);
  ServiceManager s;
  auto ins = s.instances;
  std::cout << ins.size() << std::endl;
  for (auto const &[key, val] : ins) {
    std::cout << "instances" << std::endl;
    std::cout << key << ':' << val.instance << std::endl;
    std::shared_ptr<Service> l(val.instance);
    app->PushLayer(l);
  }
  app->PushLayer<ExampleLayer>();
  app->SetMenubarCallback([app]() {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }
  });
  return app;
}
int Main(int argc, char **argv) {
  // std::cout << "Version" << AppImGuiShell_VERSION_MAJOR << "." <<
  // AppImGuiShell_VERSION_MINOR std::endl;

  while (g_ApplicationRunning) {
    AppSystem::Application *app = AppSystem::CreateApplication(argc, argv);
    app->Run();
    delete app;
  }

  return 0;
}
int main(int argc, char **argv) { return Main(argc, argv); }
