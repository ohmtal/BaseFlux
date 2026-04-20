cmake_minimum_required(VERSION 3.24)

# --- Project settings: ---
project(BaseFluxDemo)
set(PROJECT_SRC
    src/main.cpp
)

# --- CXX Settings ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#  --- BaseFlux from Directory ---
set(BASEFLUX_DIR "../" CACHE PATH "Path to BaseFlux")
if(NOT EXISTS "${BASEFLUX_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "The directory OHMFLUX_DIR='${OHMFLUX_DIR}' does not contain a CMakeLists.txt file!")
endif()

add_subdirectory("${BASEFLUX_DIR}" "${CMAKE_BINARY_DIR}/baseFlux_build")


# --- Project ---
add_executable(${PROJECT_NAME}
    ${PROJECT_SRC}
)

# test memory:
# target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -g)
# target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)

target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)




target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3-static
    ImGui
    BaseFlux
)
