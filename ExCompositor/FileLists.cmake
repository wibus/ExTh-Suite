## Headers ##

# All the header files #
SET(ExCompositor_HEADERS
    ${ExCompositor_SRC_DIR}/ExCompositorWindow.h
    ${ExCompositor_SRC_DIR}/Serializer.h
    ${ExCompositor_SRC_DIR}/Section.h)



## Sources ##

# All the source files #
SET(ExCompositor_SOURCES
    ${ExCompositor_SRC_DIR}/ExCompositorWindow.cpp
    ${ExCompositor_SRC_DIR}/Serializer.cpp
    ${ExCompositor_SRC_DIR}/Section.cpp
    ${ExCompositor_SRC_DIR}/main.cpp)



## UI
SET(ExCompositor_UI_FILES
    ${ExCompositor_SRC_DIR}/ExCompositorWindow.ui)
QT5_WRAP_UI(ExCompositor_UI_SRCS ${ExCompositor_UI_FILES})



## Global ##
SET(ExCompositor_CONFIG_FILES
    ${ExCompositor_SRC_DIR}/CMakeLists.txt
    ${ExCompositor_SRC_DIR}/FileLists.cmake
    ${ExCompositor_SRC_DIR}/LibLists.cmake)
	
SET(ExCompositor_SRC_FILES
    ${ExCompositor_HEADERS}
    ${ExCompositor_SOURCES}
    ${ExCompositor_UI_FILES}
    ${ExCompositor_CONFIG_FILES}
    ${ExCompositor_MOC_CPP_FILES})
