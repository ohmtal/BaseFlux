cmake_minimum_required(VERSION 3.24)
# -----------------------------------------------------------------------------
# Project settings - this is all you have to modify:
project(LuaFlux)
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

# =============== Lua >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
set(lua_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/lua)


set(LUA_SOURCES
    ${lua_SOURCE_DIR}/lapi.c
    ${lua_SOURCE_DIR}/lauxlib.c
    ${lua_SOURCE_DIR}/lbaselib.c
    ${lua_SOURCE_DIR}/lcode.c
    ${lua_SOURCE_DIR}/lcorolib.c
    ${lua_SOURCE_DIR}/lctype.c
    ${lua_SOURCE_DIR}/ldblib.c
    ${lua_SOURCE_DIR}/ldebug.c
    ${lua_SOURCE_DIR}/ldo.c
    ${lua_SOURCE_DIR}/ldump.c
    ${lua_SOURCE_DIR}/lfunc.c
    ${lua_SOURCE_DIR}/lgc.c
    ${lua_SOURCE_DIR}/linit.c
    ${lua_SOURCE_DIR}/liolib.c
    ${lua_SOURCE_DIR}/llex.c
    ${lua_SOURCE_DIR}/lmathlib.c
    ${lua_SOURCE_DIR}/lmem.c
    ${lua_SOURCE_DIR}/loadlib.c
    ${lua_SOURCE_DIR}/lobject.c
    ${lua_SOURCE_DIR}/lopcodes.c
    ${lua_SOURCE_DIR}/loslib.c
    ${lua_SOURCE_DIR}/lparser.c
    ${lua_SOURCE_DIR}/lstate.c
    ${lua_SOURCE_DIR}/lstring.c
    ${lua_SOURCE_DIR}/lstrlib.c
    ${lua_SOURCE_DIR}/ltable.c
    ${lua_SOURCE_DIR}/ltablib.c
    ${lua_SOURCE_DIR}/ltests.c
    ${lua_SOURCE_DIR}/ltm.c
    ${lua_SOURCE_DIR}/lundump.c
    ${lua_SOURCE_DIR}/lutf8lib.c
    ${lua_SOURCE_DIR}/lvm.c
    ${lua_SOURCE_DIR}/lzio.c
)

# COMPILE AS CXX to prevent linker errors !!!
# NOTE: lua.hpp did fix it!
# set_source_files_properties(${LUA_SOURCES} PROPERTIES LANGUAGE CXX)
add_library(lua STATIC ${LUA_SOURCES})
# set_target_properties(lua PROPERTIES LINKER_LANGUAGE CXX)

# --- generate lua.hpp -----
set(LUA_HPP_PATH "${lua_SOURCE_DIR}/lua.hpp")

# if(NOT EXISTS "${LUA_HPP_PATH}")
    # file(WRITE "${LUA_HPP_PATH}"
    #     "// XXTH lua.hpp \n"
    #     "#pragma once \n"
    #     "#include \"lua.h\"\n"
    #     "#include \"lualib.h\"\n"
    #     "#include \"lauxlib.h\"\n"
    # )

    file(WRITE "${LUA_HPP_PATH}"
      "extern \"C\" { \n"
          "#include \"lua.h\"\n"
          "#include \"lualib.h\"\n"
          "#include \"lauxlib.h\"\n"
          "}\n"
    )

    message(STATUS "generated: ${LUA_HPP_PATH}")
# endif()


# ------- lua compile parameter:
if (WIN32)
  target_compile_definitions(lua PRIVATE LUA_USE_WINDOWS)
endif()

if (UNIX)
  target_compile_definitions(lua PRIVATE LUA_USE_LINUX)
  target_link_libraries(lua PRIVATE dl)
endif()


# <<<<<<<<<<<<<<<<<<<<<<<<<<<< Lua ========================


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
  "${lua_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    BaseFlux
    lua
)

# # IMPORTANT FIX SO SOL2 FIND THE CORREXT lua.h !
# if(MSVC)
#     target_compile_options(${PROJECT_NAME} PRIVATE /FI"lua.h")
# else()
#     target_compile_options(${PROJECT_NAME} PRIVATE -include "lua.h")
# endif()

