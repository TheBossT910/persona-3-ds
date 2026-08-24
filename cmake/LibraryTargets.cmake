add_library(p3d_game STATIC ${P3D_GAME_LIBRARY_SOURCES})

if(P3D_GENERATED_DIALOGUE_SOURCES)
    set_source_files_properties(${P3D_GENERATED_DIALOGUE_SOURCES} PROPERTIES GENERATED TRUE)
    target_sources(p3d_game PRIVATE ${P3D_GENERATED_DIALOGUE_SOURCES})
endif()

set_target_properties(p3d_game PROPERTIES CXX_EXTENSIONS ON)
target_compile_features(p3d_game PUBLIC cxx_std_17)

if(TARGET aegis_engine)
    target_link_libraries(p3d_game PUBLIC aegis_engine)
endif()

target_sources(p3d_game
    PUBLIC
    FILE_SET p3d_game_headers TYPE HEADERS
    BASE_DIRS ${CMAKE_SOURCE_DIR}/source
    FILES ${P3D_GAME_HEADERS}
)

target_include_directories(p3d_game
    PUBLIC
    ${P3D_ENGINE_INCLUDE_DIRS}
)
