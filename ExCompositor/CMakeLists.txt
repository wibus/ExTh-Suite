CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

SET(ExCompositor_PROJECT ExCompositor)
MESSAGE(STATUS "Building ${ExCompositor_PROJECT}")
PROJECT(${ExCompositor_PROJECT} CXX)

SET(ExCompositor_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "${ExCompositor_PROJECT} src dir: ${ExCompositor_SRC_DIR}")
SET(ExCompositor_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})
MESSAGE(STATUS "${ExCompositor_PROJECT} bin dir: ${ExCompositor_BIN_DIR}")


INCLUDE(LibLists.cmake)
INCLUDE(FileLists.cmake)

MESSAGE(STATUS "${ExCompositor_PROJECT} libraires: ${ExCompositor_LIBRARIES}")
MESSAGE(STATUS "${ExCompositor_PROJECT} Qt modules: ${ExCompositor_QT_MODULES}")
MESSAGE(STATUS "${ExCompositor_PROJECT} include dirs: ${ExCompositor_INCLUDE_DIRS}")

INCLUDE_DIRECTORIES(${ExCompositor_INCLUDE_DIRS})
ADD_EXECUTABLE(${ExCompositor_PROJECT} ${ExCompositor_SRC_FILES})
TARGET_LINK_LIBRARIES(${ExCompositor_PROJECT} ${ExCompositor_LIBRARIES})
QT5_USE_MODULES(${ExCompositor_PROJECT} ${ExCompositor_QT_MODULES})
