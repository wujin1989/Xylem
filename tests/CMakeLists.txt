cmake_minimum_required(VERSION 3.16)

project(tests LANGUAGES C)

add_executable(test-heap "test-heap.c")
target_link_libraries(test-heap PUBLIC xylem)
add_test(NAME heap COMMAND test-heap)

if(XYLEM_ENABLE_ASAN)
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
        target_compile_options(test-heap PRIVATE -fsanitize=address -fno-omit-frame-pointer)
		target_link_options(test-heap PRIVATE -fsanitize=address)
    endif()
endif()

if(XYLEM_ENABLE_COVERAGE AND (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang"))
    target_link_options(test-heap PRIVATE --coverage)
endif()