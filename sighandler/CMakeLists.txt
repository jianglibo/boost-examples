set(APP_NAME sighandler)


add_executable(${APP_NAME} 
    src/main.cpp
    )

# target_include_directories(${BBSERVER_APP_NAME} 
#     PRIVATE include
#     PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
#     PRIVATE ${MUSTACHE_INCLUDE_DIRS}
#     )


# target_link_libraries(${APP_NAME} 
#     PRIVATE Boost::asio
#     PRIVATE Boost::beast
#     PRIVATE Boost::url
#     PRIVATE Boost::json
#     PRIVATE Boost::uuid
#     PRIVATE Boost::process
#     PRIVATE Boost::iostreams
#     PRIVATE Boost::program_options
#     PRIVATE date::date date::date-tz
#     PRIVATE ZLIB::ZLIB 
#     PRIVATE OpenSSL::SSL 
#     PRIVATE RocksDB::rocksdb
#     PRIVATE OpenSSL::Crypto 
#     PRIVATE protobuf::libprotoc
#     PRIVATE protobuf::libprotobuf
#     PRIVATE fmt::fmt
#     PRIVATE TIFF::TIFF
#     PRIVATE JPEG::JPEG
#     PRIVATE unofficial-sodium::sodium
#     PRIVATE unofficial::libgit2::libgit2
#     PRIVATE ${SHARED_LIB_NAME}
#     )
