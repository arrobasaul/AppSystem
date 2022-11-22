#pragma once
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Loggin.hpp"
#include <iostream>
#include <unordered_set>
#include <vector>
namespace AppEngine {
    class Instance{
public:
#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif
    VkInstance m_Instance;
    Instance(const char* name);
    ~Instance();

private:
    bool checkValidationLayerSupport();
    void hasGflwRequiredInstanceExtensions();
    std::vector<const char *> getRequiredExtensions();
private:
    VkDebugUtilsMessengerEXT  debugMessenger{nullptr};
    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    };
}