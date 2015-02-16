cmake_minimum_required (VERSION 2.6)

project (ship-control)

option (BUILD_TESTS "Build tests" ON)

set (COMMON_CXX_FLAGS "-std=c++11")
set (CMAKE_BUILD_TYPE RelWithDebInfo)
set (CMAKE_LINKER_FLAGS -ldl)
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${COMMON_CXX_FLAGS} -ggdb")
set (CMAKE_CXX_FLAGS_RELEASE ${COMMON_CXX_FLAGS})

include_directories (${ship-control_SOURCE_DIR})

set (SHIPCONTROL_SRC Log.cpp
                     SysLog.cpp
                     MaestroCmd.cpp
                     MaestroController.cpp)

if (BUILD_TESTS)
    set (TESTS_SRC ${SHIPCONTROL_SRC} test/main.cpp
                                      test/maestro_test.cpp)
    find_library (GTEST_LIB NAMES gtest)
    if (${GTEST_LIB} EQUAL "GTEST_LIB-NOTFOUND")
        message(FATAL_ERROR "Google Test not found")
    endif (${GTEST_LIB} EQUAL "GTEST_LIB-NOTFOUND")
    add_executable (ship-control-test ${TESTS_SRC})
    target_link_libraries (ship-control-test ${GTEST_LIB})
endif (BUILD_TESTS)
