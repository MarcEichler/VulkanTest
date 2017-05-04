cmake_minimum_required(VERSION 3.7.1)
project(pENGuin)

set(CMAKE_CXX_STANDARD 11)

# Set include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add source files
file(GLOB_RECURSE SOURCE_FILES source/*.cpp)
add_executable(pENGuin ${SOURCE_FILES})

# Set library pre- and suffix
set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".so")

option(AUTO_LOCATE_VULKAN "Try to utomatically locate Vulkan SDK" ON)

# Find Vulkan Path using CMake's Vulkan Module
# This will return Boolean 'Vulkan_FOUND' indicating the status of find as success(ON) or fail(OFF).
# Include directory path - 'Vulkan_INCLUDE_DIRS' and 'Vulkan_LIBRARY' with required libraries.
message(${AUTO_LOCATE_VULKAN})
if (AUTO_LOCATE_VULKAN)
    message(STATUS "Attempting to find Vulkan")
    find_package(Vulkan)
else()
    set(Vulkan_FOUND FALSE)
endif()

if(NOT Vulkan_FOUND)
    message(STATUS "Attempting to locate Vulkan SDK using manual path...")
    set(VULKAN_SDK "$ENV{HOME}/VulkanSDK")
    set(VULKAN_VERSION "1.0.46.0")
    set(VULKAN_ARCH "x86_64")
    set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/${VULKAN_ARCH}")
    message(STATUS "Using manual specified path: ${VULKAN_PATH}")

    # Check if manual set path exists
    if(NOT EXISTS "${VULKAN_PATH}")
        message("Error: Unable to locate Vulkan SDK at ${VULKAN_PATH}, please specify the correct path.")
        return()
    else()
        message(STATUS "Found Vulkan in ${VULKAN_PATH}")
    endif()
else()
    message(STATUS "Found Vulkan in ${Vulkan_INCLUDE_DIRS} and ${Vulkan_LIBRARY}")
endif()

# Add any required preprocessor definitions and libraries
if (UNIX)
    add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
    set(VULKAN_LIB_LIST "vulkan")
endif(UNIX)
if (WIN32)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
    set(VULKAN_LIB_LIST "vulkan-1")
endif(WIN32)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Include Vulkan header files from Vulkan SDK
    include_directories(AFTER ${VULKAN_PATH}/Include)

    # Link directory for vulkan-1
    link_directories(${VULKAN_PATH}/Bin)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Include Vulkan header files from Vulkan SDK
    include_directories(AFTER ${VULKAN_PATH}/include)

    # Link directory for vulkan-1
    link_directories(${VULKAN_PATH}/lib)
endif()
