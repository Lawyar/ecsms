add_library(pipeline_stages
    inc/Int32RandomGenerator.h
    inc/Int32Visualizer.h
    inc/Int32ToDoubleConverter.h
    inc/DoubleVisualizer.h
    inc/Int32RandomGeneratorPipelineFactory.h
    
    src/Int32RandomGenerator.cpp
    src/Int32Visualizer.cpp
    src/Int32ToDoubleConverter.cpp
    src/DoubleVisualizer.cpp
    src/Int32RandomGeneratorPipelineFactory.cpp)

target_include_directories(pipeline_stages PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(pipeline_stages PUBLIC pipeline)
