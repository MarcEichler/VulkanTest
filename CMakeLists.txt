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

# AUTO_LOCATE_VULKAN - accepted value ON or OFF
# ON  - Use CMake to auto locate the Vulkan SDK.
# OFF - Vulkan SDK path can be specified manually. This is helpful to test the build on various Vulkan version.
option(AUTO_LOCATE_VULKAN "AUTO_LOCATE_VULKAN" ON)

function(find_vulkan)
    if (NOT DEFINED ENV{VULKAN_SDK})
        set(ENV{VULKAN_SDK} "~/VulkanSDK/1.0.42.0/x86_64/")
        message(STATUS "Using ${VULKAN_SDK}")
    endif()

    # Find Vulkan Path using CMake's Vulkan Module
    # This will return Boolean 'Vulkan_FOUND' indicating the status of find as success(ON) or fail(OFF).
    # Include directory path - 'Vulkan_INCLUDE_DIRS' and 'Vulkan_LIBRARY' with required libraries.
    find_package(Vulkan)

    if(NOT Vulkan_FOUND)
        # Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIRS}
        if (NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
            set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
            STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
        endif()
        # CMake may fail to locate the libraries but could be able to
        # provide some path in Vulkan SDK include directory variable
        # 'Vulkan_INCLUDE_DIRS', try to extract path from this.
        message(STATUS "Failed to locate Vulkan SDK, retrying again...")
        if(EXISTS "${VULKAN_PATH}")
            message(STATUS "Successfully located the Vulkan SDK: ${VULKAN_PATH}")
        else()
            message("Error: Unable to locate Vulkan SDK. Please turn off auto locate option by specifying 'AUTO_LOCATE_VULKAN' as 'OFF'")
            message("and specify manually path using 'VULKAN_SDK' and 'VULKAN_VERSION' variables in the CMakeLists.txt.")
            return()
        endif()
    endif()
endfunction(find_vulkan)

if(AUTO_LOCATE_VULKAN)
    message(STATUS "Attempting to auto locate Vulkan...")
    find_vulkan()
else()
    message(STATUS "Attempting to locate Vulkan SDK using manual path...")
    set(VULKAN_SDK "~/VulkanSDK")
    set(VULKAN_VERSION "1.0.42.0")
    set(VULKAN_ARCH "x86_64")
    set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/${VULKAN_ARCH}")
    message(STATUS "Using manual specified path: ${VULKAN_PATH}")

    # Check if manual set path exists
    if(NOT EXISTS "${VULKAN_PATH}")
        message("Error: Unable to locate this Vulkan SDK path VULKAN_PATH: ${VULKAN_PATH}, please specify correct path.")
        return()
    endif()
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
