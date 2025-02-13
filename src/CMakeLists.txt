cmake_minimum_required(VERSION 3.10)

# Define the project name
project(Aurora)

find_package(Vulkan REQUIRED)

message(${Vulkan_FOUND}) 
message(${Vulkan_INCLUDE_DIRS}) 
message(${Vulkan_LIBRARIES}) 

# Specify the source files
set(SOURCES
    main.cpp
)
# add AURORA_PLATFORM_WINDOWS for Windows
if(WIN32)
	add_definitions(-DAURORA_PLATFORM_WINDOWS)
endif()

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})