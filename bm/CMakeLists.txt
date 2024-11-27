# include google benchmark
find_package(benchmark)
find_package(fmt CONFIG REQUIRED)

find_package(Boost REQUIRED COMPONENTS url)
find_package(Boost REQUIRED COMPONENTS uuid)
find_package(Boost REQUIRED COMPONENTS json)
find_package(Boost REQUIRED COMPONENTS process)
find_package(date CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)

find_package(GTest CONFIG REQUIRED)

function(add_bm_executable BM_NAME)

add_executable(${BM_NAME} ${BM_NAME} )
# target_include_directories(${BM_NAME})

target_link_libraries(
    ${BM_NAME}
    PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
    PRIVATE fmt::fmt
    PRIVATE Boost::url
    PRIVATE Boost::json
    PRIVATE Boost::uuid
    PRIVATE date::date date::date-tz
    PRIVATE benchmark::benchmark
    PRIVATE benchmark::benchmark_main
    )
endfunction()

add_bm_executable(first_benchmark.cpp)