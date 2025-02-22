cmake_minimum_required(VERSION 3.10)

# Define the project name
project(Aurora)

find_package(Vulkan REQUIRED)

message(${Vulkan_FOUND}) 
message(${Vulkan_INCLUDE_DIRS}) 
message(${Vulkan_LIBRARIES}) 

set(CMAKE_CXX_STANDARD 20)      # Use C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Enforce C++20
set(CMAKE_CXX_EXTENSIONS OFF)   # Disable compiler-specific extensions

# Specify the source files
file(GLOB_RECURSE SOURCES "./*.cpp" "./*.h")

# add AURORA_PLATFORM_WINDOWS for Windows
if(WIN32)
	add_definitions(-DAURORA_PLATFORM_WINDOWS)
endif()

# Create the executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src)
target_link_libraries(Aurora ${Vulkan_LIBRARIES})

# add a release configuration
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)