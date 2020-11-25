cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# We force C++ 17
set(CMAKE_CXX_STANDARD 17)

# Name of the project
project(LevelOneAdventure)

# General Includes
include_directories(
	"include" 
	"src/header"
	"res/shaders")

# Add new header files here
set(HEADER_FILES ${HEADER_FILES} 
	"src/header/Graphics/Attrib.h"
	"src/header/Graphics/BasicRenderer.h"
	"src/header/Graphics/Geometry.h"
	"src/header/Graphics/GLSLProgram.h"
	"src/header/Graphics/Material.h"
	"src/header/Graphics/Mesh.h"
	"src/header/Graphics/MeshCache.h"
	"src/header/Graphics/preamble.h"
	"src/header/Graphics/RenderStateKey.h"
	"src/header/Graphics/Scene.h"
	"src/header/Graphics/ShaderSet.h"
	"src/header/Graphics/TEX.h"
	"src/header/Graphics/TEXCache.h"
	"src/header/Graphics/VAO.h"
	"src/header/Graphics/VBO.h"
	"src/header/Systems/System.h"
	"src/header/Systems/MovementSystem.h"
	"src/header/Systems/RenderSystem.h"
	"src/header/Util/PackedFreeList.h"
	"src/header/common.h"
	"src/header/Components.h"
	"src/header/Engine.h"
	"src/header/PlayState.h"


)

# Add new source files here
set(SOURCE_FILES ${SOURCE_FILES} 
	"src/source/Graphics/Attrib.cpp"
	"src/source/Graphics/BasicRenderer.cpp"
	"src/source/Graphics/Geometry.cpp"
	"src/source/Graphics/GLSLProgram.cpp"
	"src/source/Graphics/RenderStateKey.cpp"
	"src/source/Graphics/Scene.cpp"
	"src/source/Graphics/ShaderSet.cpp"
	"src/source/Graphics/TEX.cpp"
	"src/source/Graphics/VAO.cpp"
	"src/source/Graphics/VBO.cpp"
	"src/source/Systems/MovementSystem.cpp"
	"src/source/Systems/RenderSystem.cpp"
	"src/source/Engine.cpp"
	"src/source/PlayState.cpp"
	"src/source/main.cpp"
)


set(RES_FILES ${RES_FILES}
	"res/shaders/basic.vert"
	"res/shaders/basic.frag"
)

# Generate executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} )

# ASSIMP
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory("thirdparty/assimp") 
include_directories(
	"thirdparty/assimp/include")


# ENTT
add_subdirectory("thirdparty/entt")
include_directories(
	"thirdparty/entt/src")

# GLM
include_directories(
	"thirdparty/glm")

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("thirdparty/glfw") 

# OpenGL
find_package(OpenGL REQUIRED)

# Link libs
set(LIBS 
	EnTT::EnTT
	assimp
	glfw 
	"${CMAKE_SOURCE_DIR}/lib/glew32.lib"
	OpenGL::GL)

target_link_libraries(${PROJECT_NAME} ${LIBS})

# Copy dlls
if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/dlls"
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()


# Scan through resource folder for updated files and copy if none existing or changed
set(RES_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/res")
file (GLOB_RECURSE resources "res/*.*")
foreach(resource ${resources})
	get_filename_component(filename ${resource} NAME)
	get_filename_component(dir ${resource} DIRECTORY)
	get_filename_component(dirname ${dir} NAME)

	file(RELATIVE_PATH rel_path ${RES_FILE_PATH} ${dir})
 
	set(output "${CMAKE_CURRENT_BINARY_DIR}/res/${rel_path}/${filename}")
	message("OUTPUT_NAME ${output}")
	add_custom_command(
		COMMENT "Moving updated resource-file '${filename}'"
		OUTPUT ${output}
		DEPENDS ${resource}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${resource}
		${output}
	)
	add_custom_target(${filename} ALL DEPENDS ${resource} ${output})
 
endforeach()

