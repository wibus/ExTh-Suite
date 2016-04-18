CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

SET(ExAnimator_PROJECT ExAnimator)
MESSAGE(STATUS "Building ${ExAnimator_PROJECT}")
PROJECT(${ExAnimator_PROJECT} CXX)

SET(ExAnimator_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "${ExAnimator_PROJECT} src dir: ${ExAnimator_SRC_DIR}")
SET(ExAnimator_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})
MESSAGE(STATUS "${ExAnimator_PROJECT} bin dir: ${ExAnimator_BIN_DIR}")


INCLUDE(LibLists.cmake)
INCLUDE(FileLists.cmake)

MESSAGE(STATUS "${ExAnimator_PROJECT} libraires: ${ExAnimator_LIBRARIES}")
MESSAGE(STATUS "${ExAnimator_PROJECT} Qt modules: ${ExAnimator_QT_MODULES}")
MESSAGE(STATUS "${ExAnimator_PROJECT} include dirs: ${ExAnimator_INCLUDE_DIRS}")

INCLUDE_DIRECTORIES(${ExAnimator_INCLUDE_DIRS})
ADD_EXECUTABLE(${ExAnimator_PROJECT} ${ExAnimator_SRC_FILES})
TARGET_LINK_LIBRARIES(${ExAnimator_PROJECT} ${ExAnimator_LIBRARIES})
QT5_USE_MODULES(${ExAnimator_PROJECT} ${ExAnimator_QT_MODULES})
