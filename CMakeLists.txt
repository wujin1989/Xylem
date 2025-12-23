cmake_minimum_required(VERSION 3.16)

project(Xylem LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(xylem-utils)

option(XYLEM_ENABLE_TESTING "enable unit testing" ON)
option(XYLEM_ENABLE_ASAN "enable memory error detection" ON)
option(XYLEM_ENABLE_TSAN  "enable data race detection" OFF)
option(XYLEM_ENABLE_UBSAN "enable undefined behavior detection" ON)
option(XYLEM_ENABLE_DYNAMIC_LIBRARY "build dynamic library" OFF)
option(XYLEM_ENABLE_COVERAGE "enable code coverage reporting" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 11)

if(WIN32)
	add_compile_options("/experimental:c11atomics")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(SRCS
#	src/xylem-list.c
	src/xylem-heap.c
#	src/xylem-sha1.c
#	src/xylem-queue.c
#	src/xylem-utils.c
#	src/xylem-stack.c
	src/xylem-bswap.c
	src/xylem-rbtree.c
	src/xylem-varint.c
#	src/xylem-sha256.c
	src/xylem-base64.c
#	src/xylem-ringbuf.c
#	src/xylem-thrdpool.c
	src/xylem-waitgroup.c
)

if(WIN32)
	list(APPEND SRCS 

	)
endif()

if(UNIX)
	list(APPEND SRCS 

	)
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(TARGET_ARCH "AMD64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    set(TARGET_ARCH "aarch64")
else()
    message(FATAL_ERROR "unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(OPENSSL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT}/include")

add_library(ssl STATIC IMPORTED)
add_library(crypto STATIC IMPORTED)

if(WIN32)
    set(SSL_LIB_PATH "${OPENSSL_ROOT}/lib/${TARGET_ARCH}/${CMAKE_SYSTEM_NAME}/ssl.lib")
    set(CRYPTO_LIB_PATH "${OPENSSL_ROOT}/lib/${TARGET_ARCH}/${CMAKE_SYSTEM_NAME}/crypto.lib")
    set(EXTRA_LINK_LIBS "Crypt32.lib")
elseif(UNIX)
    set(SSL_LIB_PATH "${OPENSSL_ROOT}/lib/${TARGET_ARCH}/${CMAKE_SYSTEM_NAME}/libssl.a")
    set(CRYPTO_LIB_PATH "${OPENSSL_ROOT}/lib/${TARGET_ARCH}/${CMAKE_SYSTEM_NAME}/libcrypto.a")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

set_target_properties(ssl PROPERTIES
    IMPORTED_LOCATION "${SSL_LIB_PATH}"
    INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}"
)

set_target_properties(crypto PROPERTIES
    IMPORTED_LOCATION "${CRYPTO_LIB_PATH}"
    INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${EXTRA_LINK_LIBS}"
)

if(XYLEM_ENABLE_DYNAMIC_LIBRARY)
    add_library(xylem SHARED ${SRCS})
else()
    add_library(xylem STATIC ${SRCS})
endif()

target_link_libraries(xylem PRIVATE ssl crypto ${CMAKE_DL_LIBS})

xylem_apply_sanitizer(xylem XYLEM_ENABLE_ASAN address)
xylem_apply_sanitizer(xylem XYLEM_ENABLE_TSAN thread)
xylem_apply_sanitizer(xylem XYLEM_ENABLE_UBSAN undefined)

if(XYLEM_ENABLE_COVERAGE AND UNIX)
    target_compile_options(xylem PRIVATE --coverage)
    target_link_options(xylem PRIVATE --coverage)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION . FILES_MATCHING PATTERN "*")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs DESTINATION . FILES_MATCHING PATTERN "*")
install(TARGETS xylem DESTINATION lib)

add_subdirectory(examples)

if(XYLEM_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()

if(XYLEM_ENABLE_COVERAGE AND UNIX)
    find_program(LCOV_BIN lcov)
    find_program(GENHTML_BIN genhtml)
    if(LCOV_BIN AND GENHTML_BIN)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
            COMMAND ctest --test-dir ${CMAKE_BINARY_DIR} -C $<CONFIG> --output-on-failure
            COMMAND ${LCOV_BIN} --capture --directory . --output-file ${CMAKE_BINARY_DIR}/coverage/coverage.info
            COMMAND ${GENHTML_BIN} ${CMAKE_BINARY_DIR}/coverage/coverage.info --output-directory ${CMAKE_BINARY_DIR}/coverage/html
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "generating html coverage report in coverage/html/index.html"
        )
    else()
        message(WARNING "lcov or genhtml not found; 'coverage' target not created.")
    endif()
endif()
