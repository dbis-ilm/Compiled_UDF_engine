cmake_minimum_required(VERSION 3.10)

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(Python_ADDITIONAL_VERSIONS 3.6)
add_definitions(-DPROFILE)
# set the project name
project(FastUDF)


find_package(PythonInterp)
find_package(PythonLibs)
# print_all_variables()

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})

# add the libraries
add_subdirectory(src)
add_subdirectory(src/ops)
add_subdirectory(src/utils)

# add the executable
add_executable(example main.c)

target_link_libraries(example PUBLIC Src Ops Utils ${CMAKE_DL_LIBS} python3.6m m)

enable_testing()
add_subdirectory(test)

add_subdirectory(exp)
