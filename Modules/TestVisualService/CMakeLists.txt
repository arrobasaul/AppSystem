#find_package( 
 #   Boost 1.79 REQUIRED 
  #  COMPONENTS  json 
#)
add_library(TestVisualService SHARED src/TestVisualService.cpp)
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)
include_directories(${CMAKE_SOURCE_DIR}/App.GuiFront/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor/glm)
include_directories(${CMAKE_SOURCE_DIR}/vendor/irrKlang/include)

include_directories(${CMAKE_SOURCE_DIR}/Modules/MNetWork/include)

#add_library( IrrKlang SHARED IMPORTED GLOBAL)
#set_target_properties( IrrKlang PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/irrKlang/bin/linux-gcc-64/libIrrKlang.so )

target_link_libraries(TestVisualService MContracts IrrKlang)
install(TARGETS TestVisualService
    LIBRARY DESTINATION bin/modules
    ARCHIVE DESTINATION bin/modules)