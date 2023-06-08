if(NOT DEFINED is_multi_config)
    get_cmake_property(is_multi_config GENERATOR_IS_MULTI_CONFIG)
endif()
if(is_multi_config)     # Multi-Config Generator
    set(output_dir "${CMAKE_BINARY_DIR}/$<CONFIG>")
else(is_multi_config)   # Single-Config Generator
    set(output_dir "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
endif(is_multi_config)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${output_dir}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${output_dir}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${output_dir}/lib")