

set(target_name grid_map_${PROJECT_NAME})
add_executable(${target_name}
    src/grid_map.cu
)
target_link_libraries(${target_name} PRIVATE rtac_cuda)

