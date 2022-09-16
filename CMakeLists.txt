cmake_minimum_required(VERSION 3.2.2)

project(globalProject)



# GLFW
set(GLFW_DIR vendor/glfw) # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)

add_subdirectory(vendor/glfw)




add_subdirectory(MContracts)
add_subdirectory(App.GuiFront)
add_subdirectory(AppImGuiCore)
add_subdirectory(AppImGuiShell)
add_subdirectory(Modules/MNetWork)
# add_subdirectory(MCore)
# add_subdirectory(MCore2)