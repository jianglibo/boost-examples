cmake_minimum_required(VERSION 3.16)

cmake_policy(SET CMP0104 NEW)
cmake_policy(SET CMP0146 OLD)

project(boost-examples LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    # Use dynamic runtime libraries
    # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    # Uncomment the following lines to use static runtime libraries instead
    # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

include(cmake/CPM.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Check if ENABLE_ASAN is ON
if(ENABLE_ASAN)
    set(ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_FLAGS}")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${ASAN_FLAGS}")
    message(STATUS "AddressSanitizer enabled")
else()
    message(STATUS "AddressSanitizer disabled")
endif()


# CMAKE_BUILD_TYPE STREQUAL "Debug" and no 'NO_ASAN' environment variable
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND ENABLE_ASAN)
# Add AddressSanitizer flags
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always -v")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always -v")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always")
    endif()
else()
    message(STATUS "******AddressSanitizer is disabled******")
endif()


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_BUILD)
    # Temporarily disable debug information in Debug mode
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DNDEBUG")
    set(CMAKE_C_FLAGS_DEBUG "-O0 -DNDEBUG")
    option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Test")
     add_definitions(-DTEST_ENV)
else()
     add_definitions(-DRELEASE_BUILD)
endif()

enable_testing()
add_definitions(-DEIGEN_NO_DEBUG)


add_compile_definitions(BUFFERSIZE=2048)

add_subdirectory(gtest)
add_subdirectory(bm)
