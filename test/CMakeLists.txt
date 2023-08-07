project(cubing_tests)

file(GLOB TEST_SRC_FILES *.cpp)

add_executable(cube_unit_tests ${TEST_SRC_FILES} )

target_link_libraries(cube_unit_tests PRIVATE gtest gtest_main cubing_lib)