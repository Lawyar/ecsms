add_library(pipeline
    inc/ConnectablePipelineStage.h
    inc/ConsumerStage.h
    inc/InOutStageConnection.h
    inc/InStageConnection.h
    inc/OutStageConnection.h
    inc/Pipeline.h
    inc/PipelineStage.h
    inc/PipelineStageNameConverter.h
    inc/ProducerAndConsumerStage.h
    inc/ProducerStage.h
    inc/StageConnection.h
    inc/StageTask.h
    inc/TaskRetrieveStrategy.h
    inc/PipelineException.h
    inc/PipelineStageType.h
    
    src/Pipeline.cpp
    src/PipelineStage.cpp)

target_link_libraries(pipeline PUBLIC common)
target_include_directories(pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
