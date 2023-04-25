#find_package( 
 #   Boost 1.79 REQUIRED 
  #  COMPONENTS  json 
#)
find_package( OpenCV REQUIRED )

add_library(RayTracing SHARED src/RayTracing.cpp)
include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/MContracts/src)
include_directories(${CMAKE_SOURCE_DIR}/App.GuiFront/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor/glm)
include_directories(${CMAKE_SOURCE_DIR}/vendor/irrKlang/include)

include_directories(${CMAKE_SOURCE_DIR}/Modules/MNetWork/include)
include_directories(${OpenCV_INCLUDE_DIRS})



#add_library( IrrKlang SHARED IMPORTED GLOBAL)
#set_target_properties( IrrKlang PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/irrKlang/bin/linux-gcc-64/libIrrKlang.so )

target_link_libraries(RayTracing MContracts IrrKlang ${OpenCV_LIBS})
install(TARGETS RayTracing
    LIBRARY DESTINATION bin/modules
    ARCHIVE DESTINATION bin/modules)

if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)

    # list of files for which we add a copy rule
    set(data_SHADOW latino.mp4)

    foreach(item IN LISTS data_SHADOW)
        message(STATUS ${item})
        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${item}" "${CMAKE_CURRENT_BINARY_DIR}/Resources/${item}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${item}"
        )
    endforeach()
endif()