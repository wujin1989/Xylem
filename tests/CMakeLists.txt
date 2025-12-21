cmake_minimum_required(VERSION 3.16)

project(tests LANGUAGES C)

include(xylem-utils)

xylem_add_test(heap)
xylem_add_test(bswap)
xylem_add_test(base64)
xylem_add_test(rbtree)
xylem_add_test(varint)
xylem_add_test(waitgroup)

if(XYLEM_ENABLE_COVERAGE AND WIN32)
    find_program(OPENCPPCOVERAGE_BIN OpenCppCoverage)
    if(OPENCPPCOVERAGE_BIN)
        file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/src" SRC_DIR_NATIVE)
        file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/tests" TESTS_DIR_NATIVE)
        file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/coverage" COVERAGE_OUTPUT_DIR_NATIVE)

        add_custom_target(coverage
            COMMAND ${OPENCPPCOVERAGE_BIN}
                "--sources=${SRC_DIR_NATIVE}"
                "--excluded_sources=${TESTS_DIR_NATIVE}"
                "--export_type=html:${COVERAGE_OUTPUT_DIR_NATIVE}"
                "--cover_children"
                "--"
                ctest.exe --test-dir "${CMAKE_BINARY_DIR}" -C $<CONFIG> --output-on-failure
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMENT "generating HTML coverage report in coverage/index.html"
            USES_TERMINAL
        )
    else()
        message(WARNING "OpenCppCoverage not found. install it and add to PATH.")
    endif()
endif()