## HEADERS ##
SET(ExAnimator_MANAGERS_HEADERS
    ${ExAnimator_SRC_DIR}/UI/Managers/AnimationManager.h
    ${ExAnimator_SRC_DIR}/UI/Managers/TimelineManager.h
    ${ExAnimator_SRC_DIR}/UI/Managers/PostProdManager.h
    ${ExAnimator_SRC_DIR}/UI/Managers/CameraManager.h
    ${ExAnimator_SRC_DIR}/UI/Managers/PathManager.h)

SET(ExAnimator_UI_HEADERS
    ${ExAnimator_MANAGERS_HEADERS}
    ${ExAnimator_SRC_DIR}/UI/RaytracedView.h
    ${ExAnimator_SRC_DIR}/UI/RaytracerGui.h)

SET(ExAnimator_MODEL_HEADERS
    ${ExAnimator_SRC_DIR}/Model/PathModel.h
    ${ExAnimator_SRC_DIR}/Model/PathReader.h
    ${ExAnimator_SRC_DIR}/Model/PathWriter.h
    ${ExAnimator_SRC_DIR}/Model/SceneDocument.h)

SET(ExAnimator_ANIMATORS_HEADERS
    ${ExAnimator_SRC_DIR}/Animators/TheFruitChoreographer.h)

SET(ExAnimator_HEADERS
    ${ExAnimator_UI_HEADERS}
    ${ExAnimator_MODEL_HEADERS}
    ${ExAnimator_ANIMATORS_HEADERS}
    ${ExAnimator_SRC_DIR}/ExAnimatorCharacter.h)
    

## SOURCES ##
SET(ExAnimator_MANAGERS_SOURCES
    ${ExAnimator_SRC_DIR}/UI/Managers/AnimationManager.cpp
    ${ExAnimator_SRC_DIR}/UI/Managers/TimelineManager.cpp
    ${ExAnimator_SRC_DIR}/UI/Managers/PostProdManager.cpp
    ${ExAnimator_SRC_DIR}/UI/Managers/CameraManager.cpp
    ${ExAnimator_SRC_DIR}/UI/Managers/PathManager.cpp)

SET(ExAnimator_UI_SOURCES
    ${ExAnimator_MANAGERS_SOURCES}
    ${ExAnimator_SRC_DIR}/UI/RaytracedView.cpp
    ${ExAnimator_SRC_DIR}/UI/RaytracerGui.cpp)

SET(ExAnimator_MODEL_SOURCES
    ${ExAnimator_SRC_DIR}/Model/PathModel.cpp
    ${ExAnimator_SRC_DIR}/Model/PathReader.cpp
    ${ExAnimator_SRC_DIR}/Model/SceneDocument.cpp)

SET(ExAnimator_ANIMATORS_SOURCES
    ${ExAnimator_SRC_DIR}/Animators/TheFruitChoreographer.cpp
    ${ExAnimator_SRC_DIR}/Animators/TheFruitAnimation.cpp)

SET(ExAnimator_SOURCES
    ${ExAnimator_UI_SOURCES}
    ${ExAnimator_MODEL_SOURCES}
    ${ExAnimator_ANIMATORS_SOURCES}
    ${ExAnimator_SRC_DIR}/ExAnimatorCharacter.cpp
    ${ExAnimator_SRC_DIR}/main.cpp)


## Resources ##
QT5_ADD_RESOURCES(ExAnimator_RESOURCES
    ${ExAnimator_SRC_DIR}/resources/ExAnimator.qrc)


## UI
SET(ExAnimator_UI_FILES
    ${ExAnimator_SRC_DIR}/UI/RaytracerGui.ui)
QT5_WRAP_UI(ExAnimator_UI_SRCS
    ${ExAnimator_UI_FILES})



## GLOBAL ##
SET(ExAnimator_SRC_FILES
    ${ExAnimator_HEADERS}
    ${ExAnimator_SOURCES}
    ${ExAnimator_RESOURCES}
    ${ExAnimator_UI_FILES}
    ${ExAnimator_UI_SRCS})
