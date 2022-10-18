compile_gml(Escalator.gml EscalatorGML.h escalator_gml)

set(SOURCES
    EscalatorWindow.cpp
)

set(GENERATED_SOURCES
    EscalatorGML.h
)

serenity_lib(LibEscalator escalator)
target_link_libraries(LibEscalator LibC LibCore LibGUI)
