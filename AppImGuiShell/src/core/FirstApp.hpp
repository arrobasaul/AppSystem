#pragma once

#include "Descriptors.hpp"
#include "Device.hpp"
#include "GameObject.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "Service.h"
#include "Service.h"
#include <functional>
// std
#include <memory>
#include <vector>
#include "VisualService/VisualService.h"
namespace AppEngine {
class FirstApp {
 public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  FirstApp();
  ~FirstApp();

  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void run();
  void SetMenubarCallback(const std::function<void()> &menubarCallback) {
      m_MenubarCallback = menubarCallback;
    }
  template <typename T> void PushLayer() {
    static_assert(std::is_base_of<AppSystem::VisualService, T>::value,
                  "Pushed type is not subclass of Layer!");
    //m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
  }

  void PushLayer(const std::shared_ptr<AppSystem::VisualService> &layer ) {
    m_LayerStack.emplace_back(layer);
    //layer->OnAttach();
  }
 private:
  void loadGameObjects();
  void check_vk_result2(VkResult err)
  {
    if (err == 0)
      return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
      abort();
  }
  Window m_Window{WIDTH, HEIGHT, "Vulkan Tutorial"};
  Device m_Device{m_Window};
  Renderer m_Renderer{m_Window, m_Device};

  // note: order of declarations matters
  std::unique_ptr<DescriptorPool> globalPool{};
  GameObject::Map gameObjects;

  std::vector<std::shared_ptr<AppSystem::VisualService>> m_LayerStack;
  std::function<void()> m_MenubarCallback;
};
}  // namespace AppEngine
