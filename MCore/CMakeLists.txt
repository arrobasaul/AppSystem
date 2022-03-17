add_library(MCore src/Mcore.cpp)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)

target_link_libraries(MCore MContracts)
install(TARGETS MCore
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)