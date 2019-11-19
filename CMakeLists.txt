#Version of Cmake
cmake_minimum_required(VERSION 3.1)

#project name
project(GameOfLife)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(SOURCES "${SRC_DIR}/main.cpp" "${SRC_DIR}/shader.cpp"
            "${SRC_DIR}/GameOfLifeLogic.cpp"
            "${SRC_DIR}/GameOfLifeRenderer.cpp"
            "${SRC_DIR}/RleReader.cpp")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

#GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

add_subdirectory("${GLFW_DIR}")

target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
include_directories("${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

#glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad"  "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

#shader
include_directories(include)

set(IMGUI_DIR "${LIB_DIR}/imGui")
add_library("ImGui" "${IMGUI_DIR}/source/imgui.cpp"
            "${IMGUI_DIR}/source/imgui_impl_opengl3.cpp"
            "${IMGUI_DIR}/source/imgui_impl_glfw.cpp"
            "${IMGUI_DIR}/source/imgui_draw.cpp"
            "${IMGUI_DIR}/source/imgui_widgets.cpp"
            "${GLAD_DIR}/src/glad.c")
target_include_directories("ImGui" PRIVATE "${IMGUI_DIR}/include")
target_include_directories("ImGui" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${IMGUI_DIR}/include")
target_link_libraries(${PROJECT_NAME} "ImGui" "${CMAKE_DL_LIBS}")

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

