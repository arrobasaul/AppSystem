#pragma once

#include "Service.h"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "vulkan/vulkan.h"
#include "imgui.h"
#include "VisualService/VisualService.h"
void check_vk_result(VkResult err);

struct GLFWwindow;

namespace AppSystem {

struct ApplicationSpecification {
  std::string Name = "AppSystem App";
  uint32_t Width = 1600;
  uint32_t Height = 900;
};

class Application {
public:
  Application(const ApplicationSpecification &applicationSpecification =
                  ApplicationSpecification());
  ~Application();

  static Application &Get();

  void Run();
  void SetMenubarCallback(const std::function<void()> &menubarCallback) {
    m_MenubarCallback = menubarCallback;
  }

  template <typename T> void PushLayer(std::shared_ptr<ApplicationContext>  applicationContext) {
    //static_assert(std::is_base_of<VisualService, T>::value,
    //              "Pushed type is not subclass of Layer!");
    //m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
    //m_LayerStack.emplace_back(std::make_shared<VisualService>(new T()))->OnAttach(applicationContext);
    //m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach(applicationContext);
    m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach(applicationContext);
  }

  void PushLayer(const std::shared_ptr<VisualService> &layer,std::shared_ptr<ApplicationContext>  applicationContext) {
    m_LayerStack.emplace_back(layer);
    layer->OnAttach(applicationContext);
  }

  void Close();

  float GetTime();
  GLFWwindow *GetWindowHandle() const { return m_WindowHandle; }

  static VkInstance GetInstance();
  static VkPhysicalDevice GetPhysicalDevice();
  static VkDevice GetDevice();

  static VkCommandBuffer GetCommandBuffer(bool begin);
  static void FlushCommandBuffer(VkCommandBuffer commandBuffer);

  static void SubmitResourceFree(std::function<void()> &&func);

private:
  void Init();
  void Shutdown();

private:
  ApplicationSpecification m_Specification;
  GLFWwindow *m_WindowHandle = nullptr;
  bool m_Running = false;

  float m_TimeStep = 0.0f;
  float m_FrameTime = 0.0f;
  float m_LastFrameTime = 0.0f;

  std::vector<std::shared_ptr<VisualService>> m_LayerStack;
  std::function<void()> m_MenubarCallback;
};

// Implemented by CLIENT
Application *CreateApplication(int argc, char **argv);
} // namespace AppSystem