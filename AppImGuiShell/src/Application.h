#pragma once

#include "Service.h"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <fstream>

#include "vulkan/vulkan.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "IconsFontAwesome6.h"
#include "VisualService/VisualService.h"
//new Gui
#include "nfd.h"
#include "nfd_common.h"
#include "node.h"
#include "cppgen.h"
#include "utils.h"
#include "ui_new_field.h"
#include "ui_message_box.h"
#include "ui_class_wizard.h"
#include "ui_table_columns.h"
#include "ui_about_dlg.h"
#include "ui_binding.h"
#include "ui_combo_dlg.h"
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
  // Gui
  void CloseFile();
  void NewWidget(const std::string& name);
  void ShowCode();
  void SaveFile(bool thenClose);
  void DoCloseFile();
  void ReloadFiles();
  void OpenFile();
  void DoOpenFile(const std::string& path);
  void ToolbarUI();
  void NewFile();
  void ActivateTab(int i);

  void Work();
  void TabsUI();
  void HierarchyUI();
  void PropertyRowsUI(bool pr);
  void PropertyUI();
  void PopupUI();
  void Draw();
  std::pair<UINode*, size_t> FindParentIndex(UINode* node, const UINode* n);

private:
  ApplicationSpecification m_Specification;
  GLFWwindow *m_WindowHandle = nullptr;
  bool m_Running = false;

  float m_TimeStep = 0.0f;
  float m_FrameTime = 0.0f;
  float m_LastFrameTime = 0.0f;

  std::vector<std::shared_ptr<VisualService>> m_LayerStack;
  std::function<void()> m_MenubarCallback;

  struct File
  {
    std::string fname;
    CppGen codeGen;
    std::unique_ptr<UINode> rootNode;
    bool modified = false;
    fs::file_time_type time[2];
  };
  bool firstTime;
  UIContext ctx;
  std::unique_ptr<Widget> newNode;
  std::vector<File> fileTabs;
  int activeTab = -1;
  const float TB_SIZE = 40;
  const float TAB_SIZE = 25; 
  int styleIdx = 0;
  const std::string UNTITLED = "Untitled";
  std::string activeButton = "";
  int addInputCharacter = 0;
  bool pgFocused;
  struct TB_Button 
  {
    std::string label;
    std::string name;
  };
  std::vector<std::pair<std::string, std::vector<TB_Button>>> tbButtons{
    { "Standard", {
      { ICON_FA_ARROW_POINTER, "" },
      { ICON_FA_FONT, "Text" },
      { ICON_FA_AUDIO_DESCRIPTION, "Selectable" },
      { ICON_FA_CIRCLE_PLAY, "Button" }, //ICON_FA_SQUARE_PLUS
      { "[ab]", "Input" }, //ICON_FA_CLOSED_CAPTIONING
      { ICON_FA_SQUARE_CHECK, "CheckBox" },
      { ICON_FA_CIRCLE_DOT, "RadioButton" },
      { ICON_FA_SQUARE_CARET_DOWN, "Combo" },
      { ICON_FA_SLIDERS, "Slider" },
      { ICON_FA_CIRCLE_HALF_STROKE, "ColorEdit" },
      { ICON_FA_IMAGE, "Image" },
      { ICON_FA_WINDOW_MINIMIZE, "Separator" },
    }},
    { "Containers", {
      { ICON_FA_SQUARE_FULL, "Child" },
      { ICON_FA_TABLE_CELLS_LARGE, "Table" },
      { ICON_FA_ARROW_DOWN_WIDE_SHORT, "CollapsingHeader" },
      { ICON_FA_FOLDER_PLUS, "TabBar" },
    }}
    /*{ ICON_MD_NORTH_WEST, "" },
    { ICON_MD_CHECK_BOX_OUTLINE_BLANK, "child" },
    { ICON_MD_TEXT_FORMAT, "text" },
    { ICON_MD_VIEW_COMPACT_ALT, "button" },
    { ICON_MD_INPUT, "input" },
    { ICON_MD_CHECK_BOX, "checkbox" },
    { ICON_MD_RADIO_BUTTON_CHECKED, "radio" },*/
  };
};

// Implemented by CLIENT
Application *CreateApplication(int argc, char **argv);
} // namespace AppSystem