set(BIN_NAME dialog-agent)

set(DIALOG_AGENT_SRCS
    main.cpp
    Agent.h
    Agent.cpp
    AgentAdaptor.h
    AgentAdaptor.cpp
    DialogModel.h
    DialogModel.cpp
    DialogInstance.h
    DialogInstance.cpp
    ../common/DBusTypes.h
    ../common/DBusTypes.cpp
)

set_source_files_properties(${CMAKE_BINARY_DIR}/org.deepin.dde.dialog1.xml PROPERTIES
    INCLUDE DBusTypes.h
)
qt_add_dbus_interface(DIALOG_AGENT_SRCS ${CMAKE_BINARY_DIR}/org.deepin.dde.dialog1.xml DialogInterface)

qt_add_executable(${BIN_NAME})

qt_add_qml_module(${BIN_NAME}
    URI agent
    VERSION 1.0
    SOURCES
        ${DIALOG_AGENT_SRCS}
    QML_FILES
        Main.qml
)

add_dependencies(${BIN_NAME} dialog_xml)

target_link_libraries(${BIN_NAME} PRIVATE
    Qt6::Quick
    Qt6::DBus
    Dtk6::Core
    Dtk6::Gui
    fmt
)

install(TARGETS ${BIN_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
