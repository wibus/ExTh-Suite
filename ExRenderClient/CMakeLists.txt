CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

SET(ExRenderClient_PROJECT ExRenderClient)
MESSAGE(STATUS "Building ${ExRenderClient_PROJECT}")
PROJECT(${ExRenderClient_PROJECT} CXX)

SET(ExRenderClient_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "${ExRenderClient_PROJECT} src dir: ${ExRenderClient_SRC_DIR}")
SET(ExRenderClient_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})
MESSAGE(STATUS "${ExRenderClient_PROJECT} bin dir: ${ExRenderClient_BIN_DIR}")


INCLUDE(LibLists.cmake)
INCLUDE(FileLists.cmake)

MESSAGE(STATUS "${ExRenderClient_PROJECT} libraires: ${ExRenderClient_LIBRARIES}")
MESSAGE(STATUS "${ExRenderClient_PROJECT} Qt modules: ${ExRenderClient_QT_MODULES}")
MESSAGE(STATUS "${ExRenderClient_PROJECT} include dirs: ${ExRenderClient_INCLUDE_DIRS}")

INCLUDE_DIRECTORIES(${ExRenderClient_INCLUDE_DIRS})
ADD_EXECUTABLE(${ExRenderClient_PROJECT} ${ExRenderClient_SRC_FILES})
TARGET_LINK_LIBRARIES(${ExRenderClient_PROJECT} ${ExRenderClient_LIBRARIES})
QT5_USE_MODULES(${ExRenderClient_PROJECT} ${ExRenderClient_QT_MODULES})
