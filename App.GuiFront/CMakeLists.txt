
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)
include_directories(${CMAKE_SOURCE_DIR}/App.GuiFront/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor/glfw/include)

configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/imconfig.h ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imconfig.h  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui.h ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imgui.h  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui.cpp ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imgui.cpp  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_internal.h ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imgui_internal.h  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_draw.cpp ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imgui_draw.cpp   COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_tables.cpp ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imgui_tables.cpp  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_widgets.cpp ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imgui_widgets.cpp  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/imstb_rectpack.h ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imstb_rectpack.h  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/imstb_textedit.h ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imstb_textedit.h  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/imstb_truetype.h ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imstb_truetype.h  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_demo.cpp ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imgui_demo.cpp  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/backends/imgui_impl_glfw.cpp ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imgui_impl_glfw.cpp  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/backends/imgui_impl_glfw.h ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imgui_impl_glfw.h  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/backends/imgui_impl_vulkan.cpp ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imgui_impl_vulkan.cpp  COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/vendor/ImGui/backends/imgui_impl_vulkan.h ${CMAKE_SOURCE_DIR}/App.GuiFront/include/imgui_impl_vulkan.h  COPYONLY)

configure_file(${CMAKE_SOURCE_DIR}/vendor/stb/stb_image.h ${CMAKE_SOURCE_DIR}/App.GuiFront/include/stb_image.h  COPYONLY)
#set(IMGUI ${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui.cpp
#${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui.h
#${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_draw.cpp
#${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_tables.cpp 
#${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_widgets.cpp 
#${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_internal.h 
#${CMAKE_SOURCE_DIR}/vendor/ImGui/imstb_rectpack.h 
#${CMAKE_SOURCE_DIR}/vendor/ImGui/imstb_textedit.h 
#${CMAKE_SOURCE_DIR}/vendor/ImGui/imstb_truetype.h 
#${CMAKE_SOURCE_DIR}/vendor/ImGui/imgui_demo.cpp 
#${CMAKE_SOURCE_DIR}/vendor/ImGui/backends/imgui_impl_glfw.cpp 
#${CMAKE_SOURCE_DIR}/vendor/ImGui/backends/imgui_impl_glfw.h 
#${CMAKE_SOURCE_DIR}/vendor/ImGui/backends/imgui_impl_vulkan.cpp 
#${CMAKE_SOURCE_DIR}/vendor/ImGui/backends/imgui_impl_vulkan.h) 

file(GLOB sources include/*.cpp)
#set(STB ${CMAKE_SOURCE_DIR}/vendor/stb/stb_image.h)


add_library(App.GuiFront include/imconfig.h ${sources}
#src/imgui_impl_glfw.cpp
#src/imgui_impl_vulkan.cpp
#imgui.cpp
#imgui_draw.cpp
#imgui_tables.cpp
#imgui_widgets.cpp 
#imstb_rectpack.h
#imstb_textedit.h
#imstb_truetype.h
#src/stb_image.h
#imgui_demo.cpp
)
target_link_libraries(App.GuiFront glm )

install(TARGETS App.GuiFront
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)