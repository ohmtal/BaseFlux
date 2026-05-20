cmake_minimum_required(VERSION 3.24)

# --- Project settings: ---
set(KORK_DIR "${CMAKE_CURRENT_LIST_DIR}")

project(BaseFluxKork)
set(PROJECT_SRC

    ${KORK_DIR}/src/main.cpp
    ${KORK_DIR}/src/myPlatformProcess.cc

    ${KORK_DIR}/src/totp/base32.cc
    ${KORK_DIR}/src/totp/base32.h
    ${KORK_DIR}/src/totp/hmac-sha1.cc
    ${KORK_DIR}/src/totp/hmac-sha1.h
    ${KORK_DIR}/src/totp/totp.cc

)

# ----------- Kork Source ---------------

  set (KS_PATH "${KORK_DIR}/lib/korkscript")

  set (KS_INCLUDES
    ${KS_PATH}/
    ${KS_PATH}/engine
    ${KS_PATH}/torqueSim
  )

  set(KS_SRCS
      ${KS_PATH}/engine/core/findMatch.cc
      ${KS_PATH}/engine/core/hashFunction.cc
      ${KS_PATH}/engine/core/memStream.cc
      ${KS_PATH}/engine/core/nStream.cc
      ${KS_PATH}/engine/core/escape.cc

      ${KS_PATH}/engine/platform/platform.cc
      ${KS_PATH}/engine/platform/platformAssert.cc

      ${KS_PATH}/engine/console/compiledEval.cc
      ${KS_PATH}/engine/console/codeBlock.cc
      ${KS_PATH}/engine/console/consoleDoc.cc
      ${KS_PATH}/engine/console/stringStack.cc
      ${KS_PATH}/engine/console/consoleInternal.cc
      ${KS_PATH}/engine/console/consoleNamespace.cc
      ${KS_PATH}/engine/console/astNodes.cc
      ${KS_PATH}/engine/console/astAlloc.cc
      ${KS_PATH}/engine/console/compiler.cc

      ${KS_PATH}/engine/console/telnetConsole.cc
      ${KS_PATH}/engine/console/telnetDebugger.cc

      ${KS_PATH}/engine/console/consoleValue.h
      ${KS_PATH}/engine/console/simpleLexer.h
      ${KS_PATH}/engine/console/simpleParser.h
      ${KS_PATH}/engine/embed/api.h
      ${KS_PATH}/engine/embed/api.cpp
  )

  set(TORQUESIM_SRCS
      ${KS_PATH}/torqueSim/core/idGenerator.cc
      ${KS_PATH}/torqueSim/core/fileStream.cc
      ${KS_PATH}/torqueSim/core/stringUnit.cc
      ${KS_PATH}/torqueSim/core/stringTable.cc
      ${KS_PATH}/torqueSim/core/unicode.cc
      ${KS_PATH}/torqueSim/core/unicode.h
      ${KS_PATH}/torqueSim/console/console.cc
      ${KS_PATH}/torqueSim/console/console.h
      ${KS_PATH}/torqueSim/console/consoleFunctions.cc
      ${KS_PATH}/torqueSim/console/consoleLogger.cc
      ${KS_PATH}/torqueSim/console/consoleLogger.h
      ${KS_PATH}/torqueSim/console/consoleObject.cc
      ${KS_PATH}/torqueSim/console/consoleObject.h
      ${KS_PATH}/torqueSim/console/consoleTypes.cc
      ${KS_PATH}/torqueSim/console/consoleTypes.h
      ${KS_PATH}/torqueSim/console/typeValidators.cc
      ${KS_PATH}/torqueSim/sim/dynamicTypes.cc
      ${KS_PATH}/torqueSim/sim/scriptObject.cc
      ${KS_PATH}/torqueSim/sim/simBase.cc
      ${KS_PATH}/torqueSim/sim/simBase.h
      ${KS_PATH}/torqueSim/sim/simDictionary.cc
      ${KS_PATH}/torqueSim/sim/simDictionary.h
      ${KS_PATH}/torqueSim/sim/simFiberManager.cc
      ${KS_PATH}/torqueSim/sim/simFiberManager.h
      ${KS_PATH}/torqueSim/sim/simManager.cc
      # XXTH ${KS_PATH}/torqueSim/platform/basicPlatformProcess.cc
      ${KS_PATH}/torqueSim/platform/platformFileIO.cc
      ${KS_PATH}/torqueSim/platform/platformStrings.cc
      ${KS_PATH}/torqueSim/platform/platformThread.cc
      ${KS_PATH}/torqueSim/platform/platformNetAsync.cc
      ${KS_PATH}/torqueSim/platform/platformNetwork.cc
      ${KS_PATH}/torqueSim/platform/platformTelnet.cc
  )





# --- CXX Settings ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)

#  --- BaseFlux from Directory ---
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


# --- Project ---
add_executable(${PROJECT_NAME}
    ${KS_SRCS}
    ${TORQUESIM_SRCS}
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
    BaseFlux
)


# --- Kork includes ---
target_include_directories(${PROJECT_NAME} PRIVATE ${KORK_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${KS_INCLUDES})

