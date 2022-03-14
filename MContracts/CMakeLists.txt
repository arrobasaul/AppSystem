add_library(MContracts src/MContracts.cpp)

install(TARGETS MContracts
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)