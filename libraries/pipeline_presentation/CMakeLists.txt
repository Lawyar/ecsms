add_library(pipeline_presentation
    inc/PipelineRegistry.h
    inc/PipelineRegistryException.h
    inc/YamlToPipeline.h
    inc/YamlToPipelineException.h

    src/PipelineRegistry.cpp
    src/YamlToPipeline.cpp)

target_link_libraries(pipeline_presentation PUBLIC pipeline)
target_link_libraries(pipeline_presentation PUBLIC pipeline_sample)
target_include_directories(pipeline_presentation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
