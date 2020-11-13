cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# We force C++ 17
set(CMAKE_CXX_STANDARD 17)

#Name of the project
project(LevelOneAdventure)

set(HEADER_FILES ${HEADER_FILES} 
)

set(SOURCE_FILES ${SOURCE_FILES} 
"src/source/main.cpp")


add_executable(${PROJECT_NAME} ${SOURCE_FILES} )

# We need a CMAKE_DIR with some code to find external dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/glfw")

# OpenGL
# find_package(OpenGL REQUIRED)
 
# GLFW
# find_package(glfw3 CONFIG REQUIRED)
# message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

#Library Include Dir
include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src/header)


# Add .lib files
link_directories("${CMAKE_SOURCE_DIR}/lib")

# Degine what libraries we use
set(LIBS glfw3)

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})


