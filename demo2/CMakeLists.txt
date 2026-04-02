cmake_minimum_required(VERSION 3.24)
# -----------------------------------------------------------------------------
# Project settings:

project(BaseFluxDemo)
set(PROJECT_SRC
    src/main.cpp
)


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
  # --- directly from github:
  # GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  # GIT_TAG        release-3.4.2
  # --- when downloaded and extracted:
  # SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../lib/SDL-release-3.4.2"
  # --- when downloaded and not extracted:
  URL "${CMAKE_CURRENT_LIST_DIR}/../lib/SDL-release-3.4.2.tar.gz"
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

     # --- directly from github:
    # GIT_REPOSITORY https://github.com/ocornut/imgui.git
    # GIT_TAG        v1.92.6-docking

    # --- when downloaded and extracted:
    # SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../lib/imgui-1.92.6-docking"

    # --- when downloaded and not extracted:
    URL "${CMAKE_CURRENT_LIST_DIR}/../lib/imgui-1.92.6-docking.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE

)

FetchContent_MakeAvailable(imgui)

add_library(LibImGui
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_link_libraries(LibImGui PRIVATE SDL3::SDL3-static)

target_include_directories(LibImGui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)



# ------------- Main
add_executable(${PROJECT_NAME}
    ${PROJECT_SRC}
)

# test memory:
# target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -g)
# target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)

target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

# include_directory to BaseFlux:
target_include_directories(${PROJECT_NAME} PRIVATE
    ../
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3-static
    LibImGui
)
