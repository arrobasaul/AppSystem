add_library(MCore src/Mcore.cpp)

install(TARGETS MCore
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)