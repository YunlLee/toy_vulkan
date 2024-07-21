cmake_minimum_required(VERSION 3.5)

project(Vulkan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(Vulkan/Public)
include_directories(Sample/Sandbox/Public)

add_definitions(-DGC_DEFINE_RES_ROOT_DIR=\"${CMAKE_SOURCE_DIR}/Resources/\")

link_directories(${CMAKE_CURRENT_LIST_DIR}/../ThirtyLib/Vulkan/Lib)

add_subdirectory(Sample)
add_subdirectory(Vulkan)