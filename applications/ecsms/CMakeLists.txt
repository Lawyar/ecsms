set(SOURCE_LIST
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    controlls/activenodeslock.h controlls/activenodeslock.cpp
    controlls/command/addattributecommand.h controlls/command/addattributecommand.cpp
    controlls/command/addblockcommand.h controlls/command/addblockcommand.cpp
    controlls/command/addchildtagcommand.h controlls/command/addchildtagcommand.cpp
    controlls/command/addconnectioncommand.h controlls/command/addconnectioncommand.cpp
    controlls/command/addtagcommand.h controlls/command/addtagcommand.cpp
    controlls/command/attrtextchangedcommand.h controlls/command/attrtextchangedcommand.cpp
    controlls/command/commandmanager.h controlls/command/commandmanager.cpp
    controlls/command/icommand.h
    controlls/command/macrocommand.h controlls/command/macrocommand.cpp
    controlls/command/moveblockcommand.h controlls/command/moveblockcommand.cpp
    controlls/command/removeattributecommand.h controlls/command/removeattributecommand.cpp
    controlls/command/removecommand.h controlls/command/removecommand.cpp
    controlls/command/removetagcommand.h controlls/command/removetagcommand.cpp
    controlls/command/tagtextchangedcommand.h controlls/command/tagtextchangedcommand.cpp
    controlls/controllerprocedure.h
    controlls/defaultcontroller.h controlls/defaultcontroller.cpp
    controlls/drawlinecontroller.h controlls/drawlinecontroller.cpp
    controlls/drawrectanglecontroller.h controlls/drawrectanglecontroller.cpp
    controlls/emptycontroller.h controlls/emptycontroller.cpp
    controlls/icontroller.h
    delegate/qlineeditdelegate.cpp delegate/qlineeditdelegate.h
    events/addblockevent.h events/addblockevent.cpp
    events/changeactivenodeevent.h  events/changeactivenodeevent.cpp
    events/changecontrollerevent.h events/changecontrollerevent.cpp
    events/drawevent.h events/drawevent.cpp
    events/event.h
    events/removeblockevent.h events/removeblockevent.cpp
    events/repaintevent.h events/repaintevent.cpp
    events/visualmodelupdateevent.h events/visualmodelupdateevent.cpp
    events/updateblockevent.h events/updateblockevent.cpp
    models/fieldmodel.h models/fieldmodel.cpp
    models/imodel.h
    models/phantomlinemodel.h models/phantomlinemodel.cpp
    models/phantomrectanglemodel.h models/phantomrectanglemodel.cpp
    models/nodetype.h
    models/selectionmodel.h models/selectionmodel.cpp
    models/visualizationmodel.h models/visualizationmodel.cpp
    namemaker/namemaker.h  namemaker/namemaker.cpp
    namemaker/id.h  namemaker/id.cpp
    observer/iobserver.h
    utility/mainwindowutility.h
    utility/containerutility.h
    utility/selectionutility.h
    utility/tagutility.h
    widgets/blockfieldwidget.cpp widgets/blockfieldwidget.h
    widgets/blockwidget.cpp widgets/blockwidget.h
    widgets/connectnodewidget.cpp widgets/connectnodewidget.h
    widgets/parameterizeddlg.cpp widgets/parameterizeddlg.h
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(ONLY_SOURCE_LIST ${SOURCE_LIST})

qt5_add_resources(SOURCE_LIST resource.qrc)

add_executable(ecsms WIN32 ${SOURCE_LIST})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${ONLY_SOURCE_LIST})

target_link_libraries(ecsms PRIVATE
    Qt5::Charts
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

target_link_libraries(ecsms PRIVATE pipeline_presentation)
