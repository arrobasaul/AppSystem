file(GLOB sources src/core/*.cpp src/core/systems/*.cpp)
project(AppImGuiShell VERSION 1.0) 
include_directories(${CMAKE_SOURCE_DIR}/AppImGuiShell/src)
include_directories(${CMAKE_SOURCE_DIR}/AppImGuiCore/src) # include files from AppImGuiCore
include_directories(${CMAKE_SOURCE_DIR}/App.GuiFront/include)
include_directories(${CMAKE_SOURCE_DIR}/App.GuiFront)
include_directories(${CMAKE_SOURCE_DIR}/vendor/glm)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)
include_directories(${CMAKE_SOURCE_DIR}/vendor/glfw/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor/ImGui)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

# add_compile_options(-Wall -Wextra -Wpedantic -fpermissive)
# Libraries
set (Vulkan_DIR bin/debug)
find_package(Vulkan REQUIRED )
find_package( OpenCV REQUIRED )
#find_library(VULKAN_LIBRARY
#NAMES vulkan vulkan-1)
#set(LIBRARIES "glfw;${VULKAN_LIBRARY}")
set(LIBRARIES "Vulkan::Vulkan" )
message(STATUS "Vulkan: ${Vulkan_FOUND}")
# Use vulkan headers from glfw:

add_executable(AppImGuiShell ${sources} src/AppImGuiShell.cpp src/Application.cpp src/Input/Input.cpp src/Image.cpp src/Random.h src/Timer.h src/Image.h src/Input/KeyCodes.h)

#include_directories( ${OpenCV_INCLUDE_DIRS})
target_include_directories(AppImGuiShell 
PUBLIC vendor/glfw/deps
PUBLIC vendor/glfw/include
PUBLIC ${PROJECT_BINARY_DIR}
PUBLIC ${OpenCV_INCLUDE_DIRS}
)

target_link_directories(AppImGuiShell 
PRIVATE ${CMAKE_SOURCE_DIR}/out/release/vendor/glfw/src
)
target_link_libraries(AppImGuiShell AppImGuiCore App.GuiFront glfw glm IrrKlang ${LIBRARIES} ${OpenCV_LIBS})

add_library( IrrKlang SHARED IMPORTED GLOBAL)
set_target_properties( IrrKlang PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/irrKlang/bin/linux-gcc-64/libIrrKlang.so )

target_compile_definitions(AppImGuiShell PUBLIC -DImTextureID=ImU64)
install(TARGETS AppImGuiShell RUNTIME DESTINATION bin)

set (dist_gem_name modules) 
add_custom_command(TARGET AppImGuiShell POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E # Create a working directory. 
        make_directory ${dist_gem_name}
    COMMAND ${CMAKE_COMMAND} -E 
        copy_if_different ${CMAKE_SOURCE_DIR}/out/build/Modules/MNetWork/libMNetwork.so ${dist_gem_name} 
)
add_custom_command(TARGET AppImGuiShell POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E # Create a working directory. 
        make_directory ${dist_gem_name}
    COMMAND ${CMAKE_COMMAND} -E 
        copy_if_different ${CMAKE_SOURCE_DIR}/out/build/Modules/RayTracing/libRayTracing.so ${dist_gem_name} 
)
add_custom_command(TARGET AppImGuiShell POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E # Create a working directory. 
        make_directory ${dist_gem_name}
    COMMAND ${CMAKE_COMMAND} -E 
        copy_if_different ${CMAKE_SOURCE_DIR}/out/build/Modules/TestVisualService/libTestVisualService.so ${dist_gem_name} 
)


#add_custom_target (libMNetwork
 #   COMMAND ${CMAKE_COMMAND} -E # Create a working directory. 
  #      make_directory ${dist_gem_name} COMMAND ${CMAKE_COMMAND} -E # copy">copy a build system in there. 
    #copy ${CMAKE_SOURCE_DIR}/CMakeLists-gem.txt ${dist_gem_name} COMMAND ${CMAKE_COMMAND} -E # Copy the source file">file. copy ${CMAKE_SOURCE_DIR}/bindings/ruby/rubyRUBY_wrap.cxx ${dist_gem_name} COMMAND ${CMAKE_COMMAND} -E # Copy the directory with header files. copy_directory ${CMAKE_SOURCE_DIR}/include ${dist_gem_name} COMMAND ${CMAKE_COMMAND} -E # Create a distribution archive. tar czf ${dist_gem_name}.tgz ${dist_gem_name} COMMAND ${CMAKE_COMMAND} -E # Dispose of our directory. remove_directory ${dist_gem_name} DEPENDS ${CMAKE_SOURCE_DIR}/bindings/ruby/rubyRUBY_wrap.cxx 
   # COMMAND ${CMAKE_COMMAND} -E 
    #    copy ${CMAKE_SOURCE_DIR}/out/build/Modules/MNetWork/libMNetwork.so ${dist_gem_name}
    #WORKING_DIR ${CMAKE_BINARY_DIR})
    