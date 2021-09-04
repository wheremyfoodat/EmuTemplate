cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set (IMGUI_DIR ../imgui) # Set Imgui dir for imgui-sfml to work

project(MyEmulator)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(WIN32)
    set(SFML_STATIC_LIBRARIES TRUE)
elseif(APPLE)
    set(IMGUI_SFML_FIND_SFML OFF) # Make imgui-sfml work with brew SFML installs
endif()
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)

if(NOT SFML_FOUND)
  message(FATAL_ERROR "SFML couldn't be located!")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/include/GUI/)
include_directories (${SFML_INCLUDE_DIR})
include_directories(third-party/)
include_directories(third-party/imgui/)
include_directories(third-party/imgui-sfml/)
include_directories(third-party/fmt/include)
include_directories(third-party/Dolphin)
include_directories(third-party/tinyfiledialogs)
include_directories(third-party/imgui-club/imgui_memory_editor)
include_directories(third-party/json)
include_directories(third-party/sha1)
include_directories(third-party/mio/single_include)
include_directories(third-party/SaveFile/include)
include_directories(third-party/xbyak/xbyak)

add_subdirectory(third-party/imgui-sfml)

add_executable(MyEmulator
    src/main.cpp
    src/MyEmulator.cpp
    src/recompiler.cpp
    src/GUI/gui.cpp

    third-party/imgui/imgui_draw.cpp
    third-party/imgui/imgui_demo.cpp
    third-party/imgui/imgui_tables.cpp
    third-party/imgui/imgui_widgets.cpp
    third-party/imgui/imgui.cpp
    third-party/imgui-sfml/imgui-SFML.cpp
    third-party/tinyfiledialogs/tinyfiledialogs.c
    third-party/sha1/sha1.cpp
    third-party/fmt/src/os.cc
    third-party/fmt/src/format.cc
)

# set_property(TARGET MyEmulator PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE) # Enable LTO
find_package(OpenGL REQUIRED)

if(WIN32)
    target_link_libraries (MyEmulator PRIVATE sfml-system sfml-network sfml-graphics sfml-window Imm32 glu32 ${OPENGL_LIBRARY})
else()
    target_link_libraries (MyEmulator PRIVATE sfml-system sfml-network sfml-graphics sfml-window ${OPENGL_LIBRARY})
endif()
