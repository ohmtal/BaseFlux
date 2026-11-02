cmake_minimum_required(VERSION 3.24)
# -----------------------------------------------------------------------------
# Project settings - this is all you have to modify:
project(DLSFlux)
set(PROJECT_SRC
    src/main.cpp
    src/bindings.cpp
)
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
# ------------- BaseFlux
#  --- BaseFlux from Directory OR GITHUB ---
set(BASEFLUX_DIR "../" CACHE PATH "Path to BaseFlux")
if(NOT EXISTS "${BASEFLUX_DIR}/CMakeLists.txt")
  #  message(FATAL_ERROR "The directory OHMFLUX_DIR='${OHMFLUX_DIR}' does not contain a CMakeLists.txt file!")
  FetchContent_Declare(
    BaseFlux
    # --- directly from github:
    GIT_REPOSITORY https://github.com/ohmtal/BaseFlux.git
    GIT_TAG        main
  )
  FetchContent_MakeAvailable(BaseFlux)

else()
  add_subdirectory("${BASEFLUX_DIR}" "${CMAKE_BINARY_DIR}/baseFlux_build")
endif()


# TEST FETCH DLS!
  FetchContent_Declare(
    dls
    # --- directly from github:
    GIT_REPOSITORY https://github.com/ohmtal/DirtyLittleScript.git
    GIT_TAG        main
  )
  FetchContent_MakeAvailable(dls)

# NOTE since it's header only i need to add the include path


# ------------- Main
add_executable(${PROJECT_NAME}
    ${PROJECT_SRC}
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  SOL_ALL_SAFETIES_ON=1
  IMGUI_DEFINE_MATH_OPERATORS
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(${PROJECT_NAME} BEFORE PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  # NOTE  include path here:
  "${FETCHCONTENT_BASE_DIR}/dls-src/dls/"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    BaseFlux
)


