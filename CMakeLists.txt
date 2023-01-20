cmake_minimum_required(VERSION 3.5)
project(landscape)

set(CMAKE_CXX_STANDARD 11)
set(CMALE_CXX_FLAGS "-O3")
set(DEPEN_DIR "depen")

file(GLOB_RECURSE glm
    ${DEPEN_DIR}/glm/*.h
    ${DEPEN_DIR}/glm/*.cpp
    ${DEPEN_DIR}/glm/*.hpp
)

file(GLOB_RECURSE glfw
    ${DEPEN_DIR}/glfw/*.h
    ${DEPEN_DIR}/glfw/*.cpp
    ${DEPEN_DIR}/glfw/*.hpp
)


file(GLOB_RECURSE shaders
    ${DEPEN_DIR}/shaders/*.glsl
)

file(GLOB_RECURSE textures
    ${DEPEN_DIR}/textures/*.png
    ${DEPEN_DIR}/textures/*.bmp
)

file(GLOB sources
    include/*.h
    include/*.hpp
    include/*.cpp
    include/*.c
)

include_directories("include" "depen" "depen/GLAD" "sources" "glfw" "glm" "shaders" "textures")

add_executable(${PROJECT_NAME} ${glm} ${glad} ${glfw} ${sources} ${include})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)


link_directories("static")


add_subdirectory(glad)
add_subdirectory(GLFW)
add_subdirectory(stb)
target_link_libraries(${PROJECT_NAME} glfw stb)

include(GNUInstallDirs)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

install(TARGETS ${PROJECT_NAME} DESTINATION ./exefile)
install(FILES shaders/8.3.cpuheight.fs
shaders/8.3.cpuheight.vs DESTINATION ./shaders)
install(FILES textures/1.png
textures/2.png
textures/3.png
textures/4.png
textures/5.png
textures/6.png
 DESTINATION ./textures)

set_target_properties(${EDITOR_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")





