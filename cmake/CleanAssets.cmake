set(P3D_SOURCE_DIR "${P3D_SOURCE_DIR}" CACHE PATH "Project source directory")

if(NOT P3D_SOURCE_DIR)
    message(FATAL_ERROR "P3D_SOURCE_DIR must be provided")
endif()

file(GLOB_RECURSE P3D_MUSIC_OUTPUTS
    "${P3D_SOURCE_DIR}/data/music/*.pcm"
)
file(GLOB_RECURSE P3D_VIDEO_OUTPUTS
    "${P3D_SOURCE_DIR}/data/video/*.vid"
)
file(GLOB P3D_MAP_OUTPUTS
    "${P3D_SOURCE_DIR}/source/maps/*.hpp"
)
file(GLOB P3D_MODEL_OUTPUTS
    "${P3D_SOURCE_DIR}/source/models/*.hpp"
)
file(GLOB P3D_DIALOGUE_OUTPUTS
    "${P3D_SOURCE_DIR}/source/dialogue/*_dialogue.cpp"
    "${P3D_SOURCE_DIR}/source/dialogue/*_dialogue.h"
    "${P3D_SOURCE_DIR}/source/dialogue/*_dialogue.hpp"
)
file(GLOB P3D_ASSET_STAMPS
    "${P3D_SOURCE_DIR}/out/build/*/p3d_assets_*.stamp"
)

file(REMOVE
    ${P3D_MUSIC_OUTPUTS}
    ${P3D_VIDEO_OUTPUTS}
    ${P3D_MAP_OUTPUTS}
    ${P3D_MODEL_OUTPUTS}
    ${P3D_DIALOGUE_OUTPUTS}
    ${P3D_ASSET_STAMPS}
)

message(STATUS "Generated asset outputs cleaned")
