set(BIN_NAME dialog-daemon)

set(DIALOG_DAEMON_SRCS
    main.cpp
    DialogAdaptor.h
    DialogAdaptor.cpp
    ../common/DBusTypes.h
    ../common/DBusTypes.cpp
)

qt_add_executable(${BIN_NAME})

set_source_files_properties(org.freedesktop.login1.Manager.xml PROPERTIES
    INCLUDE DBusTypes.h
)
set_source_files_properties(org.freedesktop.login1.Session.xml PROPERTIES
    INCLUDE DBusTypes.h
)
set_source_files_properties(${CMAKE_BINARY_DIR}/org.deepin.dde.dialog1.agent.xml PROPERTIES
    INCLUDE DBusTypes.h
)
qt_add_dbus_interface(DIALOG_DAEMON_SRCS org.freedesktop.login1.Manager.xml Login1ManagerInterface)
qt_add_dbus_interface(DIALOG_DAEMON_SRCS org.freedesktop.login1.Session.xml Login1SessionInterface)
qt_add_dbus_interface(DIALOG_DAEMON_SRCS ${CMAKE_BINARY_DIR}/org.deepin.dde.dialog1.agent.xml AgentInterface)

add_dependencies(${BIN_NAME} agent_xml)

target_sources(${BIN_NAME} PRIVATE
    ${DIALOG_DAEMON_SRCS}
)

target_link_libraries(${BIN_NAME} PRIVATE
    Qt6::Core
    Qt6::DBus
    PkgConfig::libsystemd
    yaml-cpp
    fmt
)

install(TARGETS ${BIN_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/dde-dialog
)

configure_file(config.h.in config.h)
