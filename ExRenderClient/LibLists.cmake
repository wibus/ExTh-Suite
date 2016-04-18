# Qt
FIND_PACKAGE(Qt5OpenGL REQUIRED)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)


# ExTh
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${ExRenderClient_SRC_DIR}/../../ExperimentalTheatre/")
FIND_PACKAGE(ExperimentalTheatre REQUIRED)


# Global
SET(ExRenderClient_LIBRARIES
    ${ExTh_LIBRARIES})
SET(ExRenderClient_INCLUDE_DIRS
    ${ExRenderClient_SRC_DIR}
    ${ExTh_INCLUDE_DIRS})
SET(ExRenderClient_QT_MODULES
    OpenGL)
