find_package(Boost 1.74.0 REQUIRED COMPONENTS program_options)

add_executable(xiv-menphina
    util.cpp

    # Main should be last
    main.cpp
)

target_include_directories(xiv-menphina
    PRIVATE
    "${CMAKE_BINARY_DIR}/configured_files/include"
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}/include"
)

target_link_libraries(xiv-menphina PRIVATE Boost::program_options nlohmann_json::nlohmann_json)
