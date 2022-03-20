add_library(MCore2 src/Mcore2.cpp)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)

target_link_libraries(MCore2 MContracts)
install(TARGETS MCore2
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)