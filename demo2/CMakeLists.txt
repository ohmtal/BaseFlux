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

# ------------- BaseFlux
include(FetchContent)


# For this demo with fetch it fetch itself again - i know ;)
FetchContent_Declare(
  BaseFlux
  # --- directly from github:
  GIT_REPOSITORY https://github.com/ohmtal/BaseFlux.git
  GIT_TAG        main
)

FetchContent_MakeAvailable(BaseFlux)


# ------------- Main
add_executable(${PROJECT_NAME}
    ${PROJECT_SRC}
)


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
