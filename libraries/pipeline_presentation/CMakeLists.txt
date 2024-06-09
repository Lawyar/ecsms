add_library(pipeline_presentation
    inc/PipelineRegistry.h
    inc/PipelineRegistryException.h
    inc/PipelineToYaml.h
    inc/YamlToPipeline.h
    inc/YamlConversionException.h
    inc/YamlPipelineStage.h

    src/PipelineRegistry.cpp
    src/PipelineToYaml.cpp
    src/YamlToPipeline.cpp)
        
find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(pipeline_presentation PUBLIC yaml-cpp::yaml-cpp)

target_link_libraries(pipeline_presentation PUBLIC pipeline)
target_link_libraries(pipeline_presentation PUBLIC pipeline_sample)
target_include_directories(pipeline_presentation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
