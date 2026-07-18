cmake_minimum_required(VERSION 3.24)
# -----------------------------------------------------------------------------
# set(CMAKE_C_COMPILER "clang")
# set(CMAKE_CXX_COMPILER "clang++")
# -----------------------------------------------------------------------------
# Project settings - this is all you have to modify:
# project(baseElf LANGUAGES C CXX)
project(baseElf)
set(PROJECT_SRC
    src/main.cpp
    src/bindings_sdl.cpp
    # moved to ElfScript src/bindings_imgui.cpp
    src/myPlatformProcess.cc
    src/ConsoleTypes.cpp
)

set (ASSET_PATH ${CMAKE_CURRENT_LIST_DIR}/assets@assets)
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if (EMSCRIPTEN)

    add_link_options(
        # "-sOFFSCREEN_FRAMEBUFFER=1"
        # "-sUSE_WEBGL2=1"
        "-sALLOW_MEMORY_GROWTH=1"
        # "-sSTACK_SIZE=524288"
        # for virtual filesystem
        # "-sFORCE_FILESYSTEM=1"
        # "-sEXPORTED_RUNTIME_METHODS=['FS', 'ccall']"
        # "-sEXIT_RUNTIME=1"
    )
endif()

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

# =============== ElfScript >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
set(ELFSCRIPT_PATH "/opt/ElfScript" CACHE PATH "Path to ElfScript")
set(ELF_MODULE ${ELFSCRIPT_PATH}/ElfScript)
set(ELF_CMAKE ${ELF_MODULE}/ElfScript.cmake)


if(NOT EXISTS ${ELF_CMAKE})
    message(FATAL_ERROR
        "\n ElfScript path is not set! "
        "\n Please provide the path using: "
        "\n cmake -B build -DELFSCRIPT_PATH=/path/to/ElfScript"
    )
else()
 include(${ELF_CMAKE})

  set (ELFSCRIPT_INCLUDES
   ${ELF_MODULE}
  )
  # optinal doc exporter
  list(APPEND ELFSCRIPT_SRC ${ELF_MODULE}/addons/console/consoleDocExporter.cpp)
  # ImGui Bindings
  list(APPEND ELFSCRIPT_SRC ${ELF_MODULE}/addons/ImGui/bindings_imgui.cpp)
  # SDL3Elf
  list(APPEND ELFSCRIPT_SRC ${ELF_MODULE}/addons/SDL3/SDL3_input.cpp)
endif()
# <<<<<<<<<<<<<<<<<<<<<<<<<<<< ElfScript ========================


# ------------- Main
add_executable(${PROJECT_NAME}
    ${PROJECT_SRC}
    ${ELFSCRIPT_SRC}
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  IMGUI_DEFINE_MATH_OPERATORS
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(${PROJECT_NAME} PRIVATE
  "${ELFSCRIPT_INCLUDES}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    BaseFlux
)


# Web Configurations
if (EMSCRIPTEN)

    add_link_options(
        "-sALLOW_MEMORY_GROWTH=1"
    )


    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "index" SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PUBLIC --sUSE_SDL=3 PUBLIC --preload-file ${ASSET_PATH} PRIVATE "-lidbfs.js")


    set(TARGET_NAME ${PROJECT_NAME})

    set(CUSTOM_SHELL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/res/minshell.html")
    if(EXISTS "${CUSTOM_SHELL_FILE}")
        target_link_options(${TARGET_NAME} PRIVATE "--shell-file" "${CUSTOM_SHELL_FILE}")
        set_target_properties(${TARGET_NAME} PROPERTIES LINK_DEPENDS "${CUSTOM_SHELL_FILE}")
    endif()

endif()
