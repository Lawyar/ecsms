add_executable(hedgehog WIN32
    blockwidget.cpp blockwidget.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    qlineeditdelegate.cpp qlineeditdelegate.h
    blockfield.cpp blockfield.h
    connectnodewidget.cpp connectnodewidget.h
)
target_link_libraries(hedgehog PRIVATE
    Qt5::Charts
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)