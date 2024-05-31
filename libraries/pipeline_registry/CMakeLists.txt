add_library(pipeline_registry INTERFACE
    inc/PipelineRegistryException.h
    inc/PipelineRegistry.h)

target_link_libraries(pipeline_registry INTERFACE pipeline)
target_include_directories(pipeline_registry INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
