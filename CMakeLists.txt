cmake_minimum_required(VERSION 3.24)
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


# ------------- SDL3
include(FetchContent)

FetchContent_Declare(
  SDL3
  URL "${CMAKE_CURRENT_LIST_DIR}/lib/SDL-release-3.4.2.tar.gz"
  DOWNLOAD_EXTRACT_TIMESTAMP FALSE
)

set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)

# ------------- ImGui
FetchContent_Declare(
    imgui
    URL "${CMAKE_CURRENT_LIST_DIR}/lib/imgui-1.92.6-docking.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE
)

FetchContent_MakeAvailable(imgui)

add_library(ImGui
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_link_libraries(ImGui PRIVATE SDL3::SDL3-static)

target_include_directories(ImGui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# ------------- BaseFlux
set (baseFlux_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
add_library(BaseFlux
    ${baseFlux_SOURCE_DIR}/BaseFlux/AudioManager.h
    ${baseFlux_SOURCE_DIR}/BaseFlux/AudioManager.cpp
    ${baseFlux_SOURCE_DIR}/BaseFlux/Main.h
    ${baseFlux_SOURCE_DIR}/BaseFlux/Main.cpp
    ${baseFlux_SOURCE_DIR}/BaseFlux/Settings.h
    ${baseFlux_SOURCE_DIR}/BaseFlux/Settings.cpp
    ${baseFlux_SOURCE_DIR}/BaseFlux/TextureManager.h
    ${baseFlux_SOURCE_DIR}/BaseFlux/TextureManager.cpp
    ${baseFlux_SOURCE_DIR}/BaseFlux/Tools.h
    ${baseFlux_SOURCE_DIR}/BaseFlux/Tools.cpp
)

target_link_libraries(BaseFlux PUBLIC
    SDL3::SDL3-static
    ImGui
)
target_include_directories(BaseFlux PUBLIC
    ${baseFlux_SOURCE_DIR}
)
