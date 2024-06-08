add_library(pipeline_registry
    inc/PipelineRegistry.h
    inc/PipelineRegistryException.h
    src/PipelineRegistry.cpp)

target_link_libraries(pipeline_registry PUBLIC pipeline)
target_link_libraries(pipeline_registry PUBLIC pipeline_sample)
target_include_directories(pipeline_registry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
