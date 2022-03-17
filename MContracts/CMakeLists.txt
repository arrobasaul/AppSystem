add_library(MContracts src/MContracts.cpp src/Service.h src/Module.h src/Base.h src/VisualService/VisualService.h)

install(TARGETS MContracts
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)