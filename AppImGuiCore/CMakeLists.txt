
include_directories(${CMAKE_SOURCE_DIR}/AppImGuiCore/src)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)
add_library(AppImGuiCore src/AppImGuiCore.cpp src/IOCContainer.cpp src/ServiceManager/ServiceManager.cpp src/Utils/Event.h)

install(TARGETS AppImGuiCore
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)