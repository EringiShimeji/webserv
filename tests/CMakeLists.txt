project(webserv_tests)

set(CMAKE_CXX_STANDARD 17)
unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
unset(CMAKE_CXX_CLANG_TIDY) # disable temporarily
unset(CMAKE_EXPORT_COMPILE_COMMANDS) # disable temporarily

include(FetchContent)

# Google Test
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG f8d7d77c06936315286eb55f8de22cd23c188571 # v1.14.0
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# FakeIt
FetchContent_Declare(
        fakeit
        GIT_REPOSITORY https://github.com/eranpeer/FakeIt.git
        GIT_TAG cb39d8a053876f74dfeed66dd335d3041f142095 # 2.4.0
)
FetchContent_MakeAvailable(fakeit)

include_directories(${CMAKE_SOURCE_DIR}/internal)
include_directories(utils)

if (CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif ()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Utils
add_library(test_utils STATIC
        utils/stream_buffer_switcher.cpp
        utils/stream_buffer_switcher.hpp)

link_libraries(webserv_internal gtest_main FakeIt::FakeIt-gtest test_utils)

# Tests
add_executable(option_test option_test.cpp)
gtest_discover_tests(option_test)

add_executable(result_test result_test.cpp)
gtest_discover_tests(result_test)

add_executable(response_writer_test response_writer_test.cpp)
gtest_discover_tests(response_writer_test)

add_executable(to_string to_string_test.cpp)
gtest_discover_tests(to_string)

add_executable(string_utils_test string_utils_test.cpp)
gtest_discover_tests(string_utils_test)

add_executable(stream_utils_test stream_utils_test.cpp)
gtest_discover_tests(stream_utils_test)

add_executable(request_parser_test request_parser_test.cpp)
gtest_discover_tests(request_parser_test)

add_executable(http_method_test http_method_test.cpp)
gtest_discover_tests(http_method_test)

add_executable(context_test context_test.cpp)
gtest_discover_tests(context_test)

add_executable(fd_reader_test fd_reader_test.cpp)
gtest_discover_tests(fd_reader_test)

add_executable(buffered_reader_test buffered_reader_test.cpp)
gtest_discover_tests(buffered_reader_test)

add_executable(read_request_test read_request_test.cpp)
gtest_discover_tests(read_request_test)
