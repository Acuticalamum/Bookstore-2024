
cmake_minimum_required(VERSION 3.16)
project(Bookstore)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

include_directories(
	${PROJECT_SOURCE_DIR}/src/
)

add_executable(code src/main.cpp) 
