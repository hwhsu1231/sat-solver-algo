if(WIN32)
    if(MSVC)
        add_compile_options("/utf-8")
        add_compile_definitions("UNICODE;_UNICODE")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES GNU)
        add_compile_options("-finput-charset=utf-8")
    endif()
endif()