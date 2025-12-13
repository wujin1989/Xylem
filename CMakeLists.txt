cmake_minimum_required(VERSION 3.16)

project(Xylem LANGUAGES C)

option(XYLEM_ENABLE_TESTING "enable testing" ON)
option(XYLEM_ENABLE_DYNAMIC_LIBRARY "build dynamic library" OFF)
option(XYLEM_ENABLE_COVERAGE "enable code coverage reporting" ON)
option(XYLEM_ENABLE_ASAN "enable AddressSanitizer for memory error detection" ON)

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 11)

if(WIN32)
	if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
		add_compile_options("/experimental:c11atomics")
	endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SRCS
#	src/xylem-list.c
	src/xylem-heap.c
#	src/xylem-sha1.c
#	src/xylem-queue.c
#	src/xylem-utils.c
#	src/xylem-stack.c
#	src/xylem-bswap.c
	src/xylem-rbtree.c
#	src/xylem-varint.c
#	src/xylem-sha256.c
#	src/xylem-base64.c
#	src/xylem-thrdpool.c
#	src/xylem-waitgroup.c
#	src/xylem-ringbuffer.c
)

if(WIN32)
	list(APPEND SRCS 

	)
endif()

if(UNIX)
	list(APPEND SRCS 

	)
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
	set(CMAKE_SYSTEM_PROCESSOR "AMD64")
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
	set(CMAKE_SYSTEM_PROCESSOR "aarch64")
endif()

add_library(ssl IMPORTED STATIC)
add_library(crypto IMPORTED STATIC)

if(WIN32)
	set_target_properties(ssl PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3/lib/${CMAKE_SYSTEM_PROCESSOR}/win/ssl.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3//include")

	set_target_properties(crypto PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3/lib/${CMAKE_SYSTEM_PROCESSOR}/win/crypto.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3//include" 
		INTERFACE_LINK_LIBRARIES "Crypt32.lib")
endif()

if(UNIX)
	set_target_properties(ssl PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3/lib/${CMAKE_SYSTEM_PROCESSOR}/unix/libssl.a"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3//include")

	set_target_properties(crypto PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3/lib/${CMAKE_SYSTEM_PROCESSOR}/unix/libcrypto.a"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3//include")
endif()

if(XYLEM_ENABLE_DYNAMIC_LIBRARY)
    add_library(xylem SHARED ${SRCS})
else()
    add_library(xylem STATIC ${SRCS})
endif()

target_link_libraries(xylem PRIVATE ssl crypto ${CMAKE_DL_LIBS})

if(XYLEM_ENABLE_ASAN)
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
        target_compile_options(xylem PRIVATE -fsanitize=address -fno-omit-frame-pointer)
		target_link_options(xylem PRIVATE -fsanitize=address)
    endif()
endif()

if(XYLEM_ENABLE_COVERAGE)
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
        target_compile_options(xylem PRIVATE --coverage)
		target_link_options(xylem PRIVATE --coverage)
    endif()
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION . FILES_MATCHING PATTERN "*")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs DESTINATION . FILES_MATCHING PATTERN "*")
install(TARGETS xylem DESTINATION lib)

add_subdirectory(examples)

if(XYLEM_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()

if(XYLEM_ENABLE_COVERAGE AND (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang"))
    find_program(LCOV_BIN lcov)
    find_program(GENHTML_BIN genhtml)

    if(LCOV_BIN AND GENHTML_BIN)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
            COMMAND ${LCOV_BIN} --capture --directory . --output-file ${CMAKE_BINARY_DIR}/coverage/coverage.info
            COMMAND ${GENHTML_BIN} ${CMAKE_BINARY_DIR}/coverage/coverage.info --output-directory ${CMAKE_BINARY_DIR}/coverage/html
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "generating html coverage report in coverage/html/index.html"
        )
    else()
        message(WARNING "lcov or genhtml not found. install lcov package to generate coverage reports.")
    endif()
endif()
