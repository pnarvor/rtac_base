

if(${PNG_FOUND})
    set(target_name png_codec_${PROJECT_NAME})
    add_executable(${target_name}
        src/png_codec.cpp
    )
    target_link_libraries(${target_name} PRIVATE rtac_base)
endif()
