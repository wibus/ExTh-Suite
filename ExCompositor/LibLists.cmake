# Qt
FIND_PACKAGE(Qt5OpenGL REQUIRED)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)


# ExTh
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${ExCompositor_SRC_DIR}/../../ExperimentalTheatre/")
FIND_PACKAGE(ExperimentalTheatre REQUIRED)


# Global
SET(ExCompositor_LIBRARIES
    ${ExTh_LIBRARIES})
SET(ExCompositor_INCLUDE_DIRS
    ${ExCompositor_SRC_DIR}
    ${ExTh_INCLUDE_DIRS})
SET(ExCompositor_QT_MODULES
    OpenGL)
