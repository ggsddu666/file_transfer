cmake_minimum_required(VERSION 3.8)
project(spider)


set(CMAKE_CXX_STANDARD 11)

AUX_SOURCE_DIRECTORY(./src SRC_DIR)
INCLUDE_DIRECTORIES( include )
SET(EXECUTABLE_OUTPUT_PATH ./bin)
SET(LIBRARY_OUTPUT_PATH ./lib)
ADD_EXECUTABLE(project ${SRC_DIR})
find_package(Threads)
target_link_libraries(project ${CMAKE_THREAD_LIBS_INIT})




#set(CMAKE_CXX_STANDARD 11)

#set(SOURCE_FILES main.cpp)
#add_executable(spider ${SOURCE_FILES})