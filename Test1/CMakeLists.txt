# cmake needs this line
cmake_minimum_required(VERSION 2.8)

# Define project name
project(rect)

SET(CMAKE_C_COMPILER g++)
add_compile_options(-std=c++11)
# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV REQUIRED)

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

if(CMAKE_VERSION VERSION_LESS "2.8.11")
  # Add OpenCV headers location to your include paths
  include_directories(${OpenCV_INCLUDE_DIRS})
endif()

# Declare the executable target built from your sources
add_executable(rect rect522.cpp)
#add_executable(test1 test1.cpp)

# Link your application with OpenCV libraries
target_link_libraries(rect ${OpenCV_LIBS})
#target_link_libraries(test1 ${OpenCV_LIBS})
