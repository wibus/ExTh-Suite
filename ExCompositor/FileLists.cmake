## Headers ##

# All the header files #
SET(ExCompositor_HEADERS
    ${ExCompositor_SRC_DIR}/ExCompositorWindow.h
    ${ExCompositor_SRC_DIR}/Processor.h
    ${ExCompositor_SRC_DIR}/Serializer.h
    ${ExCompositor_SRC_DIR}/Section.h)



## Sources ##

# All the source files #
SET(ExCompositor_SOURCES
    ${ExCompositor_SRC_DIR}/ExCompositorWindow.cpp
    ${ExCompositor_SRC_DIR}/Processor.cpp
    ${ExCompositor_SRC_DIR}/Serializer.cpp
    ${ExCompositor_SRC_DIR}/Section.cpp
    ${ExCompositor_SRC_DIR}/main.cpp)

## Shaders ##
SET(ExCompositor_SHADERS
    ${ExCompositor_SRC_DIR}/resources/shaders/Fullscreen.vert
    ${ExCompositor_SRC_DIR}/resources/shaders/FireFlies.frag
    ${ExCompositor_SRC_DIR}/resources/shaders/Luminance.frag
    ${ExCompositor_SRC_DIR}/resources/shaders/Tonemapping.frag
    ${ExCompositor_SRC_DIR}/resources/shaders/BloomBlur.frag
    ${ExCompositor_SRC_DIR}/resources/shaders/BloomBlend.frag
    ${ExCompositor_SRC_DIR}/resources/shaders/Gammatize.frag)



## Resources ##
QT5_ADD_RESOURCES(ExCompositor_RESOURCES
    ${ExCompositor_SRC_DIR}/resources/ExCompositor.qrc)

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
    ${ExCompositor_SHADERS}
    ${ExCompositor_UI_FILES}
    ${ExCompositor_RESOURCES}
    ${ExCompositor_CONFIG_FILES}
    ${ExCompositor_MOC_CPP_FILES})
