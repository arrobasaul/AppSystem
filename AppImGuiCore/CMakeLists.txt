
add_library(AppImGuiCore src/AppImGuiCore.cpp src/IOCContainer.cpp src/ServiceManager/ServiceManager.cpp src/Utils/Event.h)
include_directories(${CMAKE_SOURCE_DIR}/AppImGuiCore)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)

target_link_libraries(AppImGuiCore MContracts)
install(TARGETS AppImGuiCore
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)