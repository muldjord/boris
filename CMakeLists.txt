cmake_minimum_required(VERSION 3.16)

project(Boris VERSION 4.0.0 LANGUAGES CXX)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DVERSION=${VERSION} )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

if(WIN32)
  set(APP_ICON_RESOURCE "win32_icon.rc")
endif()

# Set build type with '-DCMAKE_BUILD_TYPE=type' where type is usually 'Debug' or 'Release'.
# Default to 'Release' type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

find_package(Qt6 REQUIRED COMPONENTS Widgets Xml Network LinguistTools)

find_package(SFML 3 REQUIRED COMPONENTS Audio)

qt_standard_project_setup()

qt_add_resources(QT_RESOURCES
  boris.qrc
)

set(SOURCES
  src/mainwindow.cpp
  src/settings.cpp
  src/about.cpp
  src/boris.cpp
  src/item.cpp
  src/scripthandler.cpp
  src/behaviour.cpp
  src/main.cpp
  src/stats.cpp
  src/weather.cpp
  src/debugaction.cpp
  src/soundmixer.cpp
  src/loader.cpp
  src/netcomm.cpp
  src/bubble.cpp
  src/shadow.cpp
  src/tinyexpr.cpp
)

qt_add_executable(Boris
  ${SOURCES}
  ${QT_RESOURCES}
  ${APP_ICON_RESOURCE}
)

target_link_libraries(Boris PRIVATE Qt6::Widgets Qt6::Xml Qt6::Network SFML::Audio)

set_target_properties(Boris PROPERTIES
  COMPILE_WARNING_AS_ERROR ON
  WIN32_EXECUTABLE ON
)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/debug CACHE PATH "Install path prefix" FORCE)
  else()
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/release CACHE PATH "Install path prefix" FORCE)
  endif()
endif()

# Update translation from sources:
# - Use 'make update_translations' to update ts file
# - Use 'make release_translations' to update qm file
qt_add_translations(Boris TS_FILES boris_da_DK.ts
  SOURCES ${SOURCES}
  # LUPDATE_OPTIONS -no-obsolete
)

add_dependencies(Boris update_translations)

install(TARGETS Boris
  RUNTIME DESTINATION .
)

install(FILES
  ${CMAKE_SOURCE_DIR}/AUTHORS
  ${CMAKE_SOURCE_DIR}/LICENSE
  ${CMAKE_CURRENT_BINARY_DIR}/boris_da_DK.qm
  ${CMAKE_SOURCE_DIR}/README.md
  DESTINATION .
)

install(DIRECTORY
  ${CMAKE_SOURCE_DIR}/data
  DESTINATION .
)
