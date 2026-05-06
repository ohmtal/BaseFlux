cmake_minimum_required(VERSION 3.24)
# -----------------------------------------------------------------------------
# Project settings:

project(RayLibTest)
set(PROJECT_SRC
    src/main.cpp
)


# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
# ------------- BaseFlux
# set(BASEFLUX_DIR "../" CACHE PATH "Path to BaseFlux")
#
# # PLATFORM_DESKTOP_SDL
#
# if(NOT EXISTS "${BASEFLUX_DIR}/CMakeLists.txt")
#   FetchContent_Declare(
#     BaseFlux
#     # --- directly from github:
#     GIT_REPOSITORY https://github.com/ohmtal/BaseFlux.git
#     GIT_TAG        main
#   )
#   FetchContent_MakeAvailable(BaseFlux)
#
# else()
#   add_subdirectory("${BASEFLUX_DIR}" "${CMAKE_BINARY_DIR}/baseFlux_build")
# endif()

# ------------- RayLib
# set(PLATFORM "SDL" CACHE STRING "raylib platform" FORCE)
set(RAYLIB_DIR "${CMAKE_CURRENT_LIST_DIR}/raylib" CACHE PATH "Path to RayLib")
if(NOT EXISTS "${RAYLIB_DIR}/CMakeLists.txt")
  message(FATAL_ERROR
    "\n" ${RAYLIB_DIR}
    "\n ********** RayLib not found! *************"
  )
else()
  add_subdirectory("${RAYLIB_DIR}" "${CMAKE_BINARY_DIR}/raylib_build")

  # target_compile_definitions( raylib PUBLIC PLATFORM_DESKTOP_SDL)

endif()


# ------------- Main
add_executable(${PROJECT_NAME}
    ${PROJECT_SRC}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${RAYLIB_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/rayspice
)



target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_DEFINE_MATH_OPERATORS)



set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)


# target_compile_definitions(${PROJECT_NAME} PUBLIC FLUX_ENGINE) # fake entry for DSP
# target_compile_definitions(${PROJECT_NAME} PUBLIC FLUX_ENGINE_FAKE)


target_link_libraries(${PROJECT_NAME} PRIVATE
    # BaseFlux
    raylib
)
