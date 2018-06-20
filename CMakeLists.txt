cmake_minimum_required(VERSION 3.5)
project(Secure_Programming)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "-g")

#Otemplate preparing
add_custom_command(
        OUTPUT otemplates COMMAND
        sh ${CMAKE_SOURCE_DIR}/buildtools/otemplate.sh
)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Copy the assets files into the build directory
#file(COPY "src/assets" DESTINATION "${CMAKE_BINARY_DIR}")

include_directories("${PROJECT_SOURCE_DIR}/include")

file(GLOB TEMPLATES ${PROJECT_SOURCE_DIR}/templates/*.c)
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/lib/*.c")

add_executable(Secure_Programming ${PROJECT_SOURCE_DIR}/main.c ${SOURCES} ${TEMPLATES} otemplates)
target_link_libraries(${PROJECT_NAME} sqlite3 onion sodium)
