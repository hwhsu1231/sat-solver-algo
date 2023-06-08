# set(PROJECT_NAME "sat-solver")

# 新增可執行檔目標
add_executable(${PROJECT_NAME})

file(GLOB src_files "*.cpp")
file(GLOB hdr_files "*.h")

target_sources(${PROJECT_NAME}
    PRIVATE
        ${src_files}
        ${hdr_files}
)

# 新增連接器選項
target_link_options(${PROJECT_NAME} 
    PRIVATE 
        "$<$<CXX_COMPILER_ID:MSVC>:/STACK:4000000>"
    # PRIVATE "$<$<CXX_COMPILER_ID:GNU>:--stack,2000000>"
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        _CRT_SECURE_NO_WARNINGS
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND 
        ${CMAKE_COMMAND} -E copy_directory 
        "${CMAKE_SOURCE_DIR}/benchmark"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/benchmark"
    COMMAND_EXPAND_LISTS
)