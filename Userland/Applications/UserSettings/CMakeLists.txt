serenity_component(
    UserSettings
    RECOMMENDED
    TARGETS UserSettings
)

compile_gml(UserSettingsWidget.gml UserSettingsWidgetGML.h user_settings_widget_gml)
compile_gml(UserPropertiesGeneralTab.gml UserPropertiesGeneralTabGML.h user_properties_general_tab_gml)
compile_gml(UserPropertiesPasswordTab.gml UserPropertiesPasswordTabGML.h user_properties_password_tab_gml)
compile_gml(UserPropertiesGroupsTab.gml UserPropertiesGroupsTabGML.h user_properties_groups_tab_gml)

set(SOURCES
    main.cpp
    ConnectionToServer.cpp
    UserSettingsWidget.cpp
    UsersModel.cpp
    GroupsModel.cpp
    StringArrayModel.cpp
    UserPropertiesGeneralTab.cpp
    UserPropertiesPasswordTab.cpp
    UserPropertiesGroupsTab.cpp
)

set(GENERATED_SOURCES
    UserSettingsWidgetGML.h
    UserPropertiesGeneralTabGML.h
    UserPropertiesPasswordTabGML.h
    UserPropertiesGroupsTabGML.h
    ../../Services/AccountServer/AccountClientEndpoint.h
    ../../Services/AccountServer/AccountServerEndpoint.h
)

serenity_app(UserSettings ICON key)
target_link_libraries(UserSettings PRIVATE LibIPC LibConfig LibGUI LibCore LibMain LibGfx)
