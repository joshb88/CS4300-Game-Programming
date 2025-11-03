cmake_minimum_required(VERSION 3.28)
project(Assignment2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSION OFF)
set(CMAKE_SXX_SCAN_FOR_MODULES OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

set(IMGUI_DIR "C:/Programming_Libraries/CPP_Libraries/imgui")
set(IMGUI_SFML_DIR "C:/Programming_Libraries/CPP_Libraries/imgui-sfml")

add_library(ImGui-SFML
    ${IMGUI_SFML_DIR}/imgui-SFML.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)

target_include_directories(ImGui-SFML PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_SFML_DIR}
)

target_link_libraries(ImGui-SFML PUBLIC
    SFML::Graphics
    SFML::Window
    SFML::System
)

add_executable(Assignment1 src/main.cpp)

target_link_libraries(Assignment1 PRIVATE
    ImGui-SFML
    SFML::Graphics
    SFML::Window
    SFML::System
)

add_custom_command(
    TARGET Assignment1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Assignment1>/fonts"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/bin/fonts"
        "$<TARGET_FILE_DIR:Assignment1>/fonts"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/bin/config.txt"
        "$<TARGET_FILE_DIR:Assignment1>/config.txt"
)

if(WIN32)
    file(GLOB SFML_DLLS
        "C:/Programming_Libraries/CPP_Libraries/SFML-3.0.2/bin/*.dll"
    )
    if(SFML_DLLS)
        add_custom_command(
            TARGET Assignment1 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SFML_DLLS}
                $<TARGET_FILE_DIR:Assignment1>
        )
    endif()
endif()

