add_library(AppImGuiCore src/AppImGuiCore.cpp src/IOCContainer.cpp)
include_directories(${CMAKE_SOURCE_DIR}/AppImGuiCore)
install(TARGETS AppImGuiCore
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)