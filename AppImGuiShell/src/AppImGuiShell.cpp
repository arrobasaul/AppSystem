#include "Application.h"
//#include "Walnut/EntryPoint.h"
#include "Image.h"
#include "ServiceManager/ServiceManager.h"
#include <dirent.h>
#include <dlfcn.h>
#include <iostream>
#include <vector>
#include <memory>
#include "Image.h"
#include "Timer.h"
#include "Random.h"
#include "VisualService/VisualService.h"
#include "ApplicationContext.h"
#include "ServicePool.h"
//#include "Config.h"

bool g_ApplicationRunning = true;

class ExampleLayer : public AppSystem::VisualService {
public:
  
  bool enabled;

  void OnUIRender() override {
    ImGui::Begin("Play Render");
    ImGui::Text("Last render:  %.3fms",m_lastRenderTime);
      if(ImGui::Button("Play")){
        Render();
      }
    ImGui::End();

    //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f,0.0f));
    ImGui::Begin("ViewPort");
      m_ViewportW = ImGui::GetContentRegionAvail().x;
      m_ViewportH = ImGui::GetContentRegionAvail().y;
      if(m_Image){
        //auto img = (ImTextureID)m_Image->GetDescriptorSet();
        //ImGui::Image(img, ImVec2( (float) m_Image->GetWidth(), (float) m_Image->GetHeight() ));
      }
    ImGui::End();

    //ImGui::ShowDemoWindow();
  }
  void Render(){
    AppSystem::Timer timer;
    if (!m_Image || m_ViewportW != m_Image->GetWidth() || m_ViewportH != m_Image->GetHeight()){
      m_Image = std::make_shared<AppSystem::Image>(m_ViewportW,m_ViewportH, AppSystem::ImageFormat::RGBA);
      delete[] m_ImageData;
      m_ImageData = new uint32_t[ m_ViewportW * m_ViewportH ];
    }
    for (uint32_t i = 0; i < m_ViewportW * m_ViewportH; i++ ){
      //m_ImageData[i] = AppSystem::Random::UInt();
      m_ImageData[i] = 0xffff00ff;
    } 
    m_lastRenderTime = timer.ElapsedMillis();
    // ImGui::ShowDemoWindow();
  }
  void postInit() {}

  void enable() { enabled = true; }

  void disable() { enabled = false; }

  bool isEnabled() { return enabled; }
private:
  uint32_t m_ViewportW = 0, m_ViewportH = 0;
  std::shared_ptr<AppSystem::Image> m_Image;
  uint32_t* m_ImageData = nullptr;
  float m_lastRenderTime;
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
  ///ServicePool p(s.applicationContext->servicePool);
  std::vector<std::shared_ptr<AppSystem::VisualService>> ins = s.applicationContext->servicePool->getServices<AppSystem::VisualService>();
   
  
  std::cout << ins.size() << std::endl;
  for (auto val : ins) {
    std::cout << "instances" << std::endl;
    //std::cout << key << ':' << val.instance << std::endl;
    //std::shared_ptr<VisualService> l(dynamic_cast<VisualService*>(val.instance));
    app->PushLayer(val,s.applicationContext);
  }
  //app->PushLayer<ExampleLayer>();
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

