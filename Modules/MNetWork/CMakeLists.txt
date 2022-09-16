find_package( 
    Boost 1.79 REQUIRED 
    COMPONENTS  json 
)

add_library(MNetwork SHARED src/MNetwork.cpp)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)
include_directories(${CMAKE_SOURCE_DIR}/App.GuiFront/include)
target_link_libraries(MNetwork MContracts Boost::json)
install(TARGETS MNetwork
    LIBRARY DESTINATION bin/modules
    ARCHIVE DESTINATION bin/modules)