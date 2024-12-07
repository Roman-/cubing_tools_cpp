cmake_minimum_required(VERSION 3.22)
project(cubing_tools)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt REQUIRED)

file(GLOB_RECURSE SOURCES "src/cubing/*.cpp")
add_library(cubing_lib ${SOURCES})
target_include_directories(cubing_lib PUBLIC submodules/strutil/include src/)

target_link_libraries(cubing_lib fmt::fmt)

#add_executable(cubing_tools ${SOURCES} src/main.cpp)
#target_link_libraries(cubing_tools PRIVATE cubing_lib)

add_executable(merge_maps ${SOURCES} src/merge_maps_main.cpp)
target_link_libraries(merge_maps PRIVATE cubing_lib)

add_executable(two_sided_mosaic_augmentation ${SOURCES} src/two_sided_mosaic_augmentation.cpp)
target_link_libraries(two_sided_mosaic_augmentation PRIVATE cubing_lib)

add_executable(find_two_sided_mosaic_algs ${SOURCES} src/find_two_sided_mosaic_algs.cpp)
target_link_libraries(find_two_sided_mosaic_algs PRIVATE cubing_lib)

add_subdirectory(submodules/googletest)
add_subdirectory(test)
