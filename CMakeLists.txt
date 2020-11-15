cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# We force C++ 17
set(CMAKE_CXX_STANDARD 17)

# Name of the project
project(LevelOneAdventure)

# General Includes
include_directories(
	"include" 
	"src/header")

set(HEADER_FILES ${HEADER_FILES} 
	"src/header/header.h")

set(SOURCE_FILES ${SOURCE_FILES} 
	"src/source/main.cpp"
	"src/source/header.cpp")



# GLM
include_directories(
	"thirdparty/glm")

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("thirdparty/glfw") 

# GLEW 

# OpenGL
find_package(OpenGL REQUIRED)

# Link libs
set(LIBS 
	glfw 
	"${CMAKE_SOURCE_DIR}/lib/glew32.lib"
	OpenGL::GL)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} )

target_link_libraries(${PROJECT_NAME} ${LIBS})

# Copy dlls
if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/dlls"
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()


# Generates Game Executable
