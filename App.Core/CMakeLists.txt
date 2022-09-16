
include_directories(${CMAKE_SOURCE_DIR}/App.Core/src)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)
add_library(App.Core src/AppImGuiCore.cpp src/IOCContainer.cpp src/ServiceManager/ServiceManager.cpp src/Utils/Event.h)

target_link_libraries(App.Core MContracts)
install(TARGETS App.Core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)