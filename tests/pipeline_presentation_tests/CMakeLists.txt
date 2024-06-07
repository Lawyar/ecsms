add_executable(pipeline_presentation_tests 
    YamlToPipeline_tests.cpp
    PipelineRegistry_tests.cpp)
    
target_link_libraries(pipeline_presentation_tests PRIVATE pipeline_presentation gtest_main gmock_main)
