cmake_minimum_required(VERSION 3.15)
project(RayTracingWeekend)

set(CMAKE_CXX_STANDARD 14)

# Eigen
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})
message(STATUS "EIGEN3 library status:")
message(STATUS "    config: ${EIGEN3_ROOT_DIR}")
message(STATUS "    version: ${EIGEN3_VERSION_STRING}")
message(STATUS "    include path: ${EIGEN3_INCLUDE_DIRS}")

include_directories(external)

add_executable(RayTracingWeekend main.cpp)
add_subdirectory(Weekend)
add_subdirectory(NextWeek)
add_subdirectory(NextWeekV2)
add_subdirectory(RestLife)
add_subdirectory(RestLifeV2)