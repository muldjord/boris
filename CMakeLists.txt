cmake_minimum_required(VERSION 3.16)

project(Boris VERSION 3.7.0 LANGUAGES CXX)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DVERSION=${VERSION} )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

# Set build type defined with '-DCMAKE_ARGS=type'
if(DEFINED CMAKE_ARGS)
  string(TOLOWER "${CMAKE_ARGS}" BUILD_TYPE)
  if(BUILD_TYPE STREQUAL "debug")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
  elseif(BUILD_TYPE STREQUAL "release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  else()
    message(FATAL_ERROR "Unknown build type: ${BUILD_TYPE}")
  endif()
endif()

# Default to 'release' type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

find_package(Qt6 REQUIRED COMPONENTS Widgets Xml Network LinguistTools)

find_package(SFML 2.5 COMPONENTS audio REQUIRED)

qt_standard_project_setup()

qt_add_resources(QT_RESOURCES
  boris.qrc
)

set(SOURCES
  src/mainwindow.cpp
  src/settings.cpp
  src/soundmixer.cpp
  src/about.cpp
  src/boris.cpp
  src/item.cpp
  src/scripthandler.cpp
  src/behaviour.cpp
  src/main.cpp
  src/stats.cpp
  src/debugaction.cpp
  src/loader.cpp
  src/netcomm.cpp
  src/bubble.cpp
  src/tinyexpr.cpp
)

set(TRANSLATIONS
  boris_da_DK.ts
)

add_custom_target(update_translations
  COMMAND lupdate ${SOURCES} -ts ${TRANSLATIONS}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Updating translation files..."
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/debug)
else()
  # Ensure we have no attached console on Windows when building as non-'debug'
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/release)
endif()

qt_add_executable(Boris
  ${SOURCES}
  ${QT_RESOURCES}
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(Boris PRIVATE -g)
endif()

add_dependencies(Boris update_translations)

target_link_libraries(Boris PRIVATE Qt6::Widgets Qt6::Xml Qt6::Network sfml-audio)

set_target_properties(Boris PROPERTIES
  WIN32_EXECUTABLE ON
)
