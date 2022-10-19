serenity_component(
    AccountServer
    REQUIRED
    TARGETS AccountServer
)

compile_ipc(AccountServer.ipc AccountServerEndpoint.h)
compile_ipc(AccountClient.ipc AccountClientEndpoint.h)

compile_gml(PasswordChangeDialog.gml PasswordChangeDialogGML.h password_change_dialog_gml)

set(SOURCES
    ConnectionFromClient.cpp
    PasswordChangeDialog.cpp
    main.cpp
)

set(GENERATED_SOURCES
    AccountServerEndpoint.h
    AccountClientEndpoint.h
    PasswordChangeDialogGML.h
)

serenity_bin(AccountServer)
target_link_libraries(AccountServer PRIVATE LibCore LibEscalator LibIPC LibGfx LibGUI LibMain)
