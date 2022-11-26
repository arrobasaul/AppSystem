add_library(MContracts src/MContracts.cpp src/Layer.h src/Service.h src/Module.h src/Base.h src/VisualService/VisualService.h src/IOCContainer.h src/ioc.h src/ApplicationContext.h src/ServicePool.h)

install(TARGETS MContracts
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)