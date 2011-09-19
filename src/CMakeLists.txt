CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FIND_PACKAGE(Boost 1.44 REQUIRED filesystem program_options)
FIND_PACKAGE(LIBUSB REQUIRED)

# Files which are shared by many executables
SET(CORE_CPPFILES
    util.cpp
    Data.cpp
)

SET(CORE_LIBS ${LIBUSB_LIBRARIES} ${Boost_LIBRARIES})
INCLUDE_DIRECTORIES(${LIBUSB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

#add our own headers plus the configured ones to the include path
INCLUDE_DIRECTORIES(
        "${PROJECT_BINARY_DIR}/configured_files/"
        "${PROJECT_SOURCE_DIR}/src/"
)

#create one library so we do not have to compile n * CORE_CPPFILES
ADD_LIBRARY(corelib STATIC ${CORE_CPPFILES})
SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
TARGET_LINK_LIBRARIES(corelib ${CORE_LIBS})

#store a reference to all executables
#SET(EXECUTABLES ${EXECUTABLES})
OPTION(ACERC11XDRV_CREATEPICTUREWIDGET "A tool to create widgets for pictures" ON)
IF(ACERC11XDRV_CREATEPICTUREWIDGET)
	ADD_EXECUTABLE(createpicturewidget createpicturewidget.cpp)
	SET(EXECUTABLES ${EXECUTABLES} createpicturewidget)
	TARGET_LINK_LIBRARIES(createpicturewidget corelib ${Boost_LIBRARIES})
ENDIF()

OPTION(ACERC11XDRV_CREATEBRIGHTNESSWIDGET "A tool to create widgets for pictures" ON)
IF(ACERC11XDRV_CREATEBRIGHTNESSWIDGET)
	ADD_EXECUTABLE(createbrightnesswidget createbrightnesswidget.cpp)
	SET(EXECUTABLES ${EXECUTABLES} createbrightnesswidget)
	TARGET_LINK_LIBRARIES(createbrightnesswidget corelib ${Boost_LIBRARIES})
ENDIF()

OPTION(ACERC11XDRV_SHOWPIC "A simple tool to put a picture on the C110" OFF)
IF(ACERC11XDRV_SHOWPIC)
	MESSAGE(FATAL_ERROR "Not yet here")
	ADD_EXECUTABLE(showpic howpic.cpp)
	SET(EXECUTABLES ${EXECUTABLES} showpic)
	TARGET_LINK_LIBRARIES(showpic corelib ${CORE_LIBS})
ENDIF()

#include the test directory
IF(ENABLE_TEST)
    ADD_SUBDIRECTORY(tests)
ENDIF()
