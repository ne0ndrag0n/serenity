serenity_component(
    AccountServer
    REQUIRED
    TARGETS AccountServer
)

compile_ipc(AccountServer.ipc AccountServerEndpoint.h)
compile_ipc(AccountClient.ipc AccountClientEndpoint.h)

compile_gml(PasswordChangeDialog.gml PasswordChangeDialogGML.h password_change_dialog_gml)
compile_gml(AddUserWizardPage1.gml AddUserWizardPage1GML.h add_user_wizard_page_1_gml)
compile_gml(AddUserWizardPage2.gml AddUserWizardPage2GML.h add_user_wizard_page_2_gml)
compile_gml(AddUserWizardPage3.gml AddUserWizardPage3GML.h add_user_wizard_page_3_gml)

set(SOURCES
    ConnectionFromClient.cpp
    PasswordChangeDialog.cpp
    AddUserWizardDialog.cpp
    main.cpp
)

set(GENERATED_SOURCES
    AccountServerEndpoint.h
    AccountClientEndpoint.h
    PasswordChangeDialogGML.h
    AddUserWizardPage1GML.h
    AddUserWizardPage2GML.h
    AddUserWizardPage3GML.h
)

serenity_bin(AccountServer)
target_link_libraries(AccountServer PRIVATE LibCore LibAuth LibIPC LibGfx LibGUI LibMain)
