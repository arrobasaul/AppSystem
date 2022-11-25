#find_package( 
 #   Boost 1.79 REQUIRED 
  #  COMPONENTS  json 
#)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(MNetwork SHARED include/MNetwork.h src/MNetwork.cpp src/CoreRuntime.hpp)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)
include_directories(${CMAKE_SOURCE_DIR}/App.GuiFront/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor/glm)
target_link_libraries(MNetwork MContracts)
install(TARGETS MNetwork
    LIBRARY DESTINATION bin/modules
    ARCHIVE DESTINATION bin/modules)