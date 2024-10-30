cmake_minimum_required(VERSION 3.16)
project(saxpy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

option(USE_TEMPS "enable dumping all stages of compilation outputs for verification" OFF)

if (DEFINED ENV{HIP_PATH})
    set(HIP_PATH $ENV{HIP_PATH} CACHE PATH "Path to which HIP has been installed")
else()
    set(HIP_PATH "/opt/rocm/hip" CACHE PATH "Path to which HIP has been installed")
endif()

if (DEFINED ENV{ROCM_PATH})
    set(ROCM_PATH $ENV{ROCM_PATH} CACHE PATH "Path to which ROCm has been installed")
else()
    set(ROCM_PATH "/opt/rocm" CACHE PATH "Path to which ROCm has been installed")
endif()

set(CMAKE_PREFIX_PATH "${HIP_PATH}" "${ROCM_PATH}" ${CMAKE_PREFIX_PATH})


find_package(hip REQUIRED)
message(STATUS "Found HIP executable at: ${HIP_BIN_INSTALL_DIR}")


add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE hip::host hip::device)

if (USE_TEMPS)
    target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC --save-temps)
endif()
