function(xylem_add_test test_name)
    add_executable(test-${test_name} "test-${test_name}.c")
    target_link_libraries(test-${test_name} PRIVATE xylem)
    add_test(NAME ${test_name} COMMAND test-${test_name})

    xylem_apply_sanitizer(test-${test_name} XYLEM_ENABLE_ASAN address)
    xylem_apply_sanitizer(test-${test_name} XYLEM_ENABLE_TSAN thread)
    xylem_apply_sanitizer(test-${test_name} XYLEM_ENABLE_UBSAN undefined)

    if(XYLEM_ENABLE_COVERAGE AND UNIX)
        target_link_options(test-${test_name} PRIVATE --coverage)
    endif()
endfunction()