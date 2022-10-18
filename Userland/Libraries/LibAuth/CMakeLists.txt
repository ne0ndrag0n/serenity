compile_gml(Escalator.gml EscalatorGML.h escalator_gml)

set(SOURCES
    Auth.cpp
    EscalatorWindow.cpp
)

set(GENERATED_SOURCES
    EscalatorGML.h
)

serenity_lib(LibAuth escalator)
target_link_libraries(LibAuth PRIVATE LibC LibCore LibGfx LibGUI LibMain)
