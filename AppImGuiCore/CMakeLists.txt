
include_directories(${CMAKE_SOURCE_DIR}/AppImGuiCore/src)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)
include_directories(${CMAKE_SOURCE_DIR}/App.GuiFront/src)
add_library(AppImGuiCore src/AppImGuiCore.cpp src/IOCContainer.cpp src/ServiceManager/ServiceManager.cpp src/Utils/Event.h)

target_link_libraries(AppImGuiCore App.GuiFront)
install(TARGETS AppImGuiCore
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)