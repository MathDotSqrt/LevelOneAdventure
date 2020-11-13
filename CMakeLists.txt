cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# We force C++ 17
set(CMAKE_CXX_STANDARD 17)

#Name of the project
project(LevelOneAdventure)

set(HEADER_FILES ${HEADER_FILES} 
"src/header/header.h")

set(SOURCE_FILES ${SOURCE_FILES} 
"src/source/main.cpp"
"src/source/header.cpp")

# Generates Game Executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} )

include_directories("include" "src/header")
