CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FIND_PACKAGE(Boost 1.44 REQUIRED filesystem program_options)
FIND_PACKAGE(LIBUSB REQUIRED)
FIND_PACKAGE(ImageMagick REQUIRED COMPONENTS Magick++)
FIND_PACKAGE(X11)

# Files which are shared by many executables
SET(CORE_CPPFILES
	lib/util.cpp
	lib/USBData.cpp
	lib/ProjectorC110.cpp
	lib/ScreenshotImportX11.cpp
	lib/USB.cpp
	lib/ImageMagickUSBData.cpp
	lib/Exception.cpp
	lib/FrameCounter.cpp
)

SET(CORE_LIBS ${LIBUSB_LIBRARIES} ${Boost_LIBRARIES} ${ImageMagick_LIBRARIES} ${X11_LIBRARIES})
INCLUDE_DIRECTORIES(${LIBUSB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ImageMagick_INCLUDE_DIRS} ${X11_INCLUDE_DIR})

#add our own headers plus the configured ones to the include path
INCLUDE_DIRECTORIES(
	"${PROJECT_BINARY_DIR}/configured_files/"
	"${PROJECT_SOURCE_DIR}/src/"
	"${PROJECT_SOURCE_DIR}/src/lib/"
)

#create one library so we do not have to compile n * CORE_CPPFILES
ADD_LIBRARY(corelib STATIC ${CORE_CPPFILES})
SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
TARGET_LINK_LIBRARIES(corelib ${CORE_LIBS})

#store a reference to all executables
#SET(EXECUTABLES ${EXECUTABLES})
OPTION(ACERC11XDRV_CREATEPICTUREWIDGET "A tool to create widgets for pictures" ON)
IF(ACERC11XDRV_CREATEPICTUREWIDGET)
	ADD_EXECUTABLE(createpicturewidget tools/createpicturewidget.cpp)
	SET(EXECUTABLES ${EXECUTABLES} createpicturewidget)
	TARGET_LINK_LIBRARIES(createpicturewidget corelib)
ENDIF()

OPTION(ACERC11XDRV_CREATEBRIGHTNESSWIDGET "A tool to create widgets for pictures" ON)
IF(ACERC11XDRV_CREATEBRIGHTNESSWIDGET)
	ADD_EXECUTABLE(createbrightnesswidget tools/createbrightnesswidget.cpp)
	SET(EXECUTABLES ${EXECUTABLES} createbrightnesswidget)
	TARGET_LINK_LIBRARIES(createbrightnesswidget corelib)
ENDIF()

OPTION(ACERC11XDRV_SHOWPIC "A simple tool to put a picture on the C110" ON)
IF(ACERC11XDRV_SHOWPIC)
	ADD_EXECUTABLE(showpic tools/showpic.cpp)
	SET(EXECUTABLES ${EXECUTABLES} showpic)
	TARGET_LINK_LIBRARIES(showpic corelib)
ENDIF()

OPTION(ACERC11XDRV_GETSCREENSHOT "A simple tool to import a screenshot from a X server" ON)
IF(ACERC11XDRV_GETSCREENSHOT)
	IF(X11_FOUND)
		IF(X11_XShm_FOUND)
			ADD_EXECUTABLE(getscreenshot tools/getScreenshot.cpp)
			SET(EXECUTABLES ${EXECUTABLES} getscreenshot)
			TARGET_LINK_LIBRARIES(getscreenshot corelib)
		ELSEIF()
			MESSAGE(FATAL_ERROR "Could not find the X11 Shm extension")
		ENDIF()
	ELSEIF()
		MESSAGE(FATAL_ERROR "Could not find the X11 library")
	ENDIF()
ENDIF()

OPTION(ACERC11XDRV_DAEMON "Daemon" ON)
IF(ACERC11XDRV_DAEMON)
	ADD_EXECUTABLE(daemon tools/daemon.cpp 	lib/ProjectorC110.cpp)
	SET(EXECUTABLES ${EXECUTABLES} daemon)
	TARGET_LINK_LIBRARIES(daemon corelib)
ENDIF()

#include the test directory
IF(ENABLE_TEST)
	ADD_SUBDIRECTORY(tests)
ENDIF()
