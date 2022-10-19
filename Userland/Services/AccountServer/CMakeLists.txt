serenity_component(
    AccountServer
    REQUIRED
    TARGETS AccountServer
)

compile_ipc(AccountServer.ipc AccountServerEndpoint.h)
compile_ipc(AccountClient.ipc AccountClientEndpoint.h)

set(SOURCES
    ConnectionFromClient.cpp
    main.cpp
)

set(GENERATED_SOURCES
    AccountServerEndpoint.h
    AccountClientEndpoint.h
)

serenity_bin(AccountServer)
target_link_libraries(AccountServer LibCore LibEscalator LibIPC LibGUI LibMain)
