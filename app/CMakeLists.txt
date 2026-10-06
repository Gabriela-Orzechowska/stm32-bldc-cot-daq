cmake_minimum_required(VERSION 3.24)

project(COTDAQ VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

# ImPlot
FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG v0.17
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(implot)

# libusb (why is it so annoyingg)
if (WIN32)
    set(LIBUSB_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(LIBUSB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(LIBUSB_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        libusb
        GIT_REPOSITORY https://github.com/libusb/libusb.git
        GIT_TAG v1.0.29
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(libusb)

    # libusb's CMake target can differ between versions.
    if (TARGET usb-1.0)
        set(LIBUSB_TARGET usb-1.0)
    elseif (TARGET libusb)
        set(LIBUSB_TARGET libusb)
    elseif (TARGET libusb-1.0)
        set(LIBUSB_TARGET libusb-1.0)
    else()
        message(FATAL_ERROR
            "Could not find a libusb CMake target."
        )
    endif()

else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBUSB REQUIRED IMPORTED_TARGET libusb-1.0)
    set(LIBUSB_TARGET PkgConfig::LIBUSB)

endif()

find_package(OpenGL REQUIRED)

set(IMGUI_DIR "${imgui_SOURCE_DIR}")
set(IMPLOT_DIR "${implot_SOURCE_DIR}")

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp

    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

add_library(implot STATIC
    ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_items.cpp
    ${IMPLOT_DIR}/implot_demo.cpp
)
target_include_directories(implot PUBLIC
    ${IMPLOT_DIR}
    ${IMGUI_DIR}
)
target_link_libraries(implot PUBLIC imgui)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/renderer/imgui_render.cpp
    src/usb/usb_device.cpp
    src/usb/usb_reader.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    implot
    imgui
    glfw
    OpenGL::GL
    ${LIBUSB_TARGET}
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)
