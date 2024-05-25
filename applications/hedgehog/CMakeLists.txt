set(SOURCE_LIST
    blockfield.cpp blockfield.h
    blockwidget.cpp blockwidget.h
    connectnodewidget.cpp connectnodewidget.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    qlineeditdelegate.cpp qlineeditdelegate.h
    controlls/defaultcontroller.h controlls/defaultcontroller.cpp
    controlls/drawlinecontroller.h controlls/drawlinecontroller.cpp
    controlls/icontroller.h
    events/changecontrollerevent.h events/changecontrollerevent.cpp
    events/drawevent.h events/drawevent.cpp
    events/drawlineevent.h events/drawlineevent.cpp
    events/event.h
    events/mypaintevent.h events/mypaintevent.cpp
    models/fieldmodel.h models/fieldmodel.cpp
    models/imodel.h
    models/linemodel.h models/linemodel.cpp
    models/selectionmodel.h models/selectionmodel.cpp
    observer/iobserver.h
)

add_executable(hedgehog WIN32 ${SOURCE_LIST})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_LIST})

target_link_libraries(hedgehog PRIVATE
    Qt5::Charts
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)