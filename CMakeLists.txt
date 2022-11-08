cmake_minimum_required(VERSION 2.8.7)

project(suduo)

# set variables
set(DIR_SRCS ${PROJECT_SOURCE_DIR}/src)
set(DIR_INCLUDE ${PROJECT_SOURCE_DIR}/include)
set(DIR_TEST ${PROJECT_SOURCE_DIR}/test)

# includes
include_directories(${DIR_INCLUDE})

# libraries
link_directories(${PROJECT_SOURCE_DIR}/lib)

# outputs
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


# add_compile_options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

set(CMAKE_BUILD_TYPE Debug)

# print compile info
# set(CMAKE_VERBOSE_MAKEFILE ON)

# add all source file to the variable SRCS
aux_source_directory(${DIR_SRCS} SRCS)

# add all test source file to the variable SRCS
aux_source_directory(${DIR_TEST} TEST_SRCS)


# library
add_library(suduo ${SRCS})

# executable for test
# foreach(SRCFILE IN LISTS TEST_SRCS)
#     get_filename_component(EXENAME ${SRCFILE} NAME_WE)
#     add_executable(${EXENAME} ${SRCFILE})
#     target_link_libraries(${EXRNAME} suduo pthread)
# endforeach()


