# Qt
FIND_PACKAGE(Qt5OpenGL REQUIRED)
FIND_PACKAGE(Qt5Multimedia REQUIRED)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# ExTh
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${ExAnimator_SRC_DIR}/../../ExperimentalTheatre/")
FIND_PACKAGE(ExperimentalTheatre REQUIRED)

#Globals
SET(ExAnimator_LIBRARIES
    ${ExTh_LIBRARIES})
SET(ExAnimator_INCLUDE_DIRS
    ${ExAnimator_INCLUDE_DIRS}
    ${ExAnimator_SRC_DIR}
    ${ExTh_INCLUDE_DIRS})
SET(ExAnimator_QT_MODULES
    OpenGL
    Multimedia)

