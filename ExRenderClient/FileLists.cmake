## HEADERS ##
SET(ExRenderClient_GUI_HEADERS
    ${ExRenderClient_SRC_DIR}/GUI/ExRenderClientGui.h
    ${ExRenderClient_SRC_DIR}/GUI/ExRenderClientView.h)

SET(ExRenderClient_HEADERS
    ${ExRenderClient_GUI_HEADERS}
    ${ExRenderClient_SRC_DIR}/ExRenderClientCharacter.h)


## SOURCES ##
SET(ExRenderClient_GUI_SOURCES
    ${ExRenderClient_SRC_DIR}/GUI/ExRenderClientGui.cpp
    ${ExRenderClient_SRC_DIR}/GUI/ExRenderClientView.cpp)

SET(ExRenderClient_SOURCES
    ${ExRenderClient_GUI_SOURCES}
    ${ExRenderClient_SRC_DIR}/ExRenderClientCharacter.cpp
    ${ExRenderClient_SRC_DIR}/main.cpp)


## Resources ##
QT5_ADD_RESOURCES(ExRenderClient_RESOURCES
    ${ExRenderClient_SRC_DIR}/resources/ExRenderClient.qrc)


## UI
SET(ExRenderClient_UI_FILES
    ${ExRenderClient_SRC_DIR}/GUI/ExRenderClientGui.ui)
QT5_WRAP_UI(ExRenderClient_UI_SRCS
    ${ExRenderClient_UI_FILES})



## GLOBAL ##
SET(ExRenderClient_SRC_FILES
    ${ExRenderClient_HEADERS}
    ${ExRenderClient_SOURCES}
    ${ExRenderClient_RESOURCES}
    ${ExRenderClient_UI_FILES}
    ${ExRenderClient_UI_SRCS})
