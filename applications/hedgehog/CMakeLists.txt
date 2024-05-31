set(SOURCE_LIST
    blockfield.cpp blockfield.h
    blockwidget.cpp blockwidget.h
    connectnodewidget.cpp connectnodewidget.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    qlineeditdelegate.cpp qlineeditdelegate.h
    controlls/activenodeslock.h controlls/activenodeslock.cpp
    controlls/command/addattributecommand.h controlls/command/addattributecommand.cpp
    controlls/command/addblockcommand.h controlls/command/addblockcommand.cpp
    controlls/command/addchildtagcommand.h controlls/command/addchildtagcommand.cpp
    controlls/command/addconnectioncommand.h controlls/command/addconnectioncommand.cpp
    controlls/command/addtagcommand.h controlls/command/addtagcommand.cpp
    controlls/command/commandmanager.h controlls/command/commandmanager.cpp
    controlls/command/icommand.h
    controlls/command/moveblockcommand.h controlls/command/moveblockcommand.cpp
    controlls/command/removecommand.h controlls/command/removecommand.cpp
    controlls/command/removetagcommand.h controlls/command/removetagcommand.cpp
    controlls/controllerprocedure.h
    controlls/defaultcontroller.h controlls/defaultcontroller.cpp
    controlls/drawlinecontroller.h controlls/drawlinecontroller.cpp
    controlls/icontroller.h
    events/addblockevent.h events/addblockevent.cpp
    events/changeactivenodeevent.h  events/changeactivenodeevent.cpp
    events/changecontrollerevent.h events/changecontrollerevent.cpp
    events/drawevent.h events/drawevent.cpp
    events/event.h
    events/mypaintevent.h events/mypaintevent.cpp
    events/updateblockevent.h events/updateblockevent.cpp
    events/removeblockevent.h events/removeblockevent.cpp
    events/repaintevent.h events/repaintevent.cpp
    models/fieldmodel.h models/fieldmodel.cpp
    models/imodel.h
    models/linemodel.h models/linemodel.cpp
    models/nodetype.h
    models/selectionmodel.h models/selectionmodel.cpp
    namemaker/blocknamemaker.h  namemaker/blocknamemaker.cpp
    namemaker/id.h  namemaker/id.cpp
    observer/iobserver.h
    utility/containerutility.h
    utility/tagutility.h
)

set(ONLY_SOURCE_LIST ${SOURCE_LIST})

qt5_add_resources(SOURCE_LIST resource.qrc)

add_executable(hedgehog WIN32 ${SOURCE_LIST})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${ONLY_SOURCE_LIST})

target_link_libraries(hedgehog PRIVATE
    Qt5::Charts
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)