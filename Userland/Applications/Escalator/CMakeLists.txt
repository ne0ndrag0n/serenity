serenity_component(
    Escalator
    REQUIRED
    TARGETS Escalator
)

set(SOURCES
    main.cpp
)

serenity_app(Escalator ICON app-escalator)
target_link_libraries(Escalator PRIVATE LibCore LibDesktop LibAuth LibGfx LibGUI LibMain)
