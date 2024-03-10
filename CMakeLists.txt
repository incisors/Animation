cmake_minimum_required(VERSION 3.16)

# set project name (thus .exe name)
# make sure to refer this with ${PROJECT_NAME} everywhere else
project(menv)

# Set C++ standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Suppress warnings
add_compile_options(-Wno-deprecated-declarations)

# Add source files
# file(GLOB_RECURSE SOURCES "src/*.cpp" main.cpp)
# This will not recursively find .cpp in subfolders, so
# after you have imgui or other UI component, use
file(GLOB_RECURSE SOURCES "src/*.cpp" "include/imgui/*.cpp" "include/imgui/backend/imgui_impl_glfw.cpp" "include/imgui/backend/imgui_impl_opengl3.cpp" main.cpp)

# Add include directories
include_directories(
    include
    include/imgui
    include/imgui/backends
)

# Find Homebrew prefix
execute_process(COMMAND brew --prefix OUTPUT_VARIABLE BREW_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
set(BREW_INCLUDE_DIR "${BREW_PREFIX}/include")
set(BREW_LIB_DIR "${BREW_PREFIX}/lib")

# Add library directories
link_directories(
    ${BREW_LIB_DIR}
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Find the GLFW3 library
find_package(glfw3 REQUIRED)

# Link libraries
target_link_libraries(${PROJECT_NAME} "-framework OpenGL" glfw)

# Move assets to correct path relative to .exe
add_custom_target(CopyShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/shaders"
    "${CMAKE_BINARY_DIR}/shaders"
)
add_dependencies(${PROJECT_NAME} CopyShaders)