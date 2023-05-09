cmake_minimum_required(VERSION 3.2.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++20 -fcoroutines")
project(globalProject)



# GLFW
set(GLFW_DIR vendor/glfw) # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/nativefiledialog)




add_subdirectory(MContracts)
add_subdirectory(App.GuiFront)
add_subdirectory(AppImGuiCore)
add_subdirectory(Modules/MNetWork)
add_subdirectory(Modules/RayTracing)
add_subdirectory(Modules/TestVisualService)
add_subdirectory(AppImGuiShell)
# add_subdirectory(MCore)
# add_subdirectory(MCore2)