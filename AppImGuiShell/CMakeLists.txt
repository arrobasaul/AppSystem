file(GLOB sources *.cpp)
project(AppImGuiShell VERSION 1.0) 
include_directories(${CMAKE_SOURCE_DIR}/AppImGuiShell/src)
include_directories(${CMAKE_SOURCE_DIR}/AppImGuiCore/src) # include files from AppImGuiCore
include_directories(${CMAKE_SOURCE_DIR}/App.GuiFront/include)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

# Libraries
set (Vulkan_DIR bin/debug)
find_package(Vulkan REQUIRED )
#find_library(VULKAN_LIBRARY
#NAMES vulkan vulkan-1)
#set(LIBRARIES "glfw;${VULKAN_LIBRARY}")
set(LIBRARIES "Vulkan::Vulkan")
message(STATUS "Vulkan: ${Vulkan_FOUND}")
# Use vulkan headers from glfw:

add_executable(AppImGuiShell ${sources} src/AppImGuiShell.cpp src/Application.cpp src/Image.cpp  src/Random.cpp src/Timer.h src/Input/Input.cpp  src/Input/KeyCodes.h)

target_include_directories(AppImGuiShell 
    PUBLIC vendor/glfw/deps
    PUBLIC vendor/glfw/include
    PUBLIC ${PROJECT_BINARY_DIR}
)

target_link_directories(AppImGuiShell 
    PRIVATE ${CMAKE_SOURCE_DIR}/out/release/vendor/glfw/src
)

target_link_libraries(AppImGuiShell AppImGuiCore glfw3 ${LIBRARIES})

target_compile_definitions(AppImGuiShell PUBLIC -DImTextureID=ImU64)
install(TARGETS AppImGuiShell RUNTIME DESTINATION bin)