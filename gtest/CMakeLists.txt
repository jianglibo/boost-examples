#
# unit tests:
#

# add_definitions(-DTEST_ENV)
add_compile_definitions(TEST_ENV)

#!!!!!!!!! it's old wide behavier. using target_include_directories instead
# include_directories(${RYML_INCLUDE_DIR})
# include catch

find_package(Boost REQUIRED COMPONENTS url)
find_package(Boost REQUIRED COMPONENTS uuid)
find_package(Boost REQUIRED COMPONENTS json)
find_package(Boost REQUIRED COMPONENTS process)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(Boost REQUIRED COMPONENTS asio)
find_package(date CONFIG REQUIRED)
find_package(boost_algorithm CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)

find_package(date CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

file(COPY "${CMAKE_SOURCE_DIR}/fixtures" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

option(GTEST_COLOR "Enable colored output for gtest" ON)
# ----------------------------boost_json_test.cpp------------------------------
set(T_NAME boost_json_test)
add_executable(${T_NAME} boost_json_test.cpp)
# target_include_directories(${T_NAME})

target_link_libraries(
    ${T_NAME}
    PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
    PRIVATE fmt::fmt
    PRIVATE Boost::url
    PRIVATE Boost::json
    PRIVATE Boost::uuid
    PRIVATE Boost::program_options
    PRIVATE Boost::process
    PRIVATE Boost::algorithm
    PRIVATE date::date date::date-tz
    )
add_test(
    NAME ${T_NAME}
    COMMAND ${T_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# ----------------------------boost_url_test.cpp------------------------------
set(T_NAME boost_url_test)
add_executable(${T_NAME} boost_url_test.cpp)
# target_include_directories(${T_NAME})

target_link_libraries(
    ${T_NAME}
    PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
    PRIVATE fmt::fmt
    PRIVATE Boost::url
    PRIVATE Boost::json
    PRIVATE Boost::uuid
    PRIVATE Boost::program_options
    PRIVATE Boost::process
    PRIVATE Boost::algorithm
    PRIVATE date::date date::date-tz
    )
add_test(
    NAME ${T_NAME}
    COMMAND ${T_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# ----------------------------boost_process_test.cpp------------------------------
set(T_NAME boost_process_test)
add_executable(${T_NAME} boost_process_test.cpp)
# target_include_directories(${T_NAME})

target_link_libraries(
    ${T_NAME}
    PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
    PRIVATE Boost::asio
    PRIVATE fmt::fmt
    PRIVATE Boost::url
    PRIVATE Boost::json
    PRIVATE Boost::uuid
    PRIVATE Boost::program_options
    PRIVATE Boost::process
    
    PRIVATE Boost::algorithm
    PRIVATE date::date date::date-tz
    )
add_test(
    NAME ${T_NAME}
    COMMAND ${T_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})