
include_directories(${CMAKE_SOURCE_DIR}/AppImGuiCore/src)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/include)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)
include_directories(${CMAKE_SOURCE_DIR}/vendor/spdlog/include)

add_library(AppImGuiCore 
    src/ServiceManager/ApplicationContext.cpp
    src/ServiceManager/ServicePool.cpp
    src/ServiceManager/MessageBroker.cpp
    src/AppImGuiCore.cpp
    src/IOCContainer.cpp
    src/ServiceManager/ServiceManager.cpp
    src/Utils/Event.h
)



target_link_libraries(AppImGuiCore spdlog)

install(TARGETS AppImGuiCore
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)