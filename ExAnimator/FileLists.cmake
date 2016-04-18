## HEADERS ##
SET(ExAnimator_MANAGERS_HEADERS
    ${ExAnimator_SRC_DIR}/GUI/Managers/AnimationManager.h
    ${ExAnimator_SRC_DIR}/GUI/Managers/TimelineManager.h
    ${ExAnimator_SRC_DIR}/GUI/Managers/PostProdManager.h
    ${ExAnimator_SRC_DIR}/GUI/Managers/CameraManager.h
    ${ExAnimator_SRC_DIR}/GUI/Managers/PathManager.h)

SET(ExAnimator_GUI_HEADERS
    ${ExAnimator_MANAGERS_HEADERS}
    ${ExAnimator_SRC_DIR}/GUI/ExAnimatorView.h
    ${ExAnimator_SRC_DIR}/GUI/ExAnimatorGui.h)

SET(ExAnimator_MODEL_HEADERS
    ${ExAnimator_SRC_DIR}/Model/PathModel.h
    ${ExAnimator_SRC_DIR}/Model/PathReader.h
    ${ExAnimator_SRC_DIR}/Model/PathWriter.h
    ${ExAnimator_SRC_DIR}/Model/SceneDocument.h)

SET(ExAnimator_ANIMATORS_HEADERS
    ${ExAnimator_SRC_DIR}/Animators/TheFruitChoreographer.h)

SET(ExAnimator_HEADERS
    ${ExAnimator_GUI_HEADERS}
    ${ExAnimator_MODEL_HEADERS}
    ${ExAnimator_ANIMATORS_HEADERS}
    ${ExAnimator_SRC_DIR}/ExAnimatorCharacter.h)
    

## SOURCES ##
SET(ExAnimator_MANAGERS_SOURCES
    ${ExAnimator_SRC_DIR}/GUI/Managers/AnimationManager.cpp
    ${ExAnimator_SRC_DIR}/GUI/Managers/TimelineManager.cpp
    ${ExAnimator_SRC_DIR}/GUI/Managers/PostProdManager.cpp
    ${ExAnimator_SRC_DIR}/GUI/Managers/CameraManager.cpp
    ${ExAnimator_SRC_DIR}/GUI/Managers/PathManager.cpp)

SET(ExAnimator_GUI_SOURCES
    ${ExAnimator_MANAGERS_SOURCES}
    ${ExAnimator_SRC_DIR}/GUI/ExAnimatorView.cpp
    ${ExAnimator_SRC_DIR}/GUI/ExAnimatorGui.cpp)

SET(ExAnimator_MODEL_SOURCES
    ${ExAnimator_SRC_DIR}/Model/PathModel.cpp
    ${ExAnimator_SRC_DIR}/Model/PathReader.cpp
    ${ExAnimator_SRC_DIR}/Model/SceneDocument.cpp)

SET(ExAnimator_ANIMATORS_SOURCES
    ${ExAnimator_SRC_DIR}/Animators/TheFruitChoreographer.cpp
    ${ExAnimator_SRC_DIR}/Animators/TheFruitAnimation.cpp)

SET(ExAnimator_SOURCES
    ${ExAnimator_GUI_SOURCES}
    ${ExAnimator_MODEL_SOURCES}
    ${ExAnimator_ANIMATORS_SOURCES}
    ${ExAnimator_SRC_DIR}/ExAnimatorCharacter.cpp
    ${ExAnimator_SRC_DIR}/main.cpp)


## Resources ##
QT5_ADD_RESOURCES(ExAnimator_RESOURCES
    ${ExAnimator_SRC_DIR}/resources/ExAnimator.qrc)


## GUI
SET(ExAnimator_GUI_FILES
    ${ExAnimator_SRC_DIR}/GUI/ExAnimatorGui.ui)
QT5_WRAP_UI(ExAnimator_GUI_SRCS
    ${ExAnimator_GUI_FILES})



## GLOBAL ##
SET(ExAnimator_SRC_FILES
    ${ExAnimator_HEADERS}
    ${ExAnimator_SOURCES}
    ${ExAnimator_RESOURCES}
    ${ExAnimator_GUI_FILES}
    ${ExAnimator_GUI_SRCS})
