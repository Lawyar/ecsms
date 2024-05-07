add_library(pipeline
    inc/ConnectablePipelineStage.h
    inc/ConsumerStage.h
    inc/ConsumerTaskRetrieveStrategy.h
    inc/InOutStageConnection.h
    inc/InStageConnection.h
    inc/OutStageConnection.h
    src/Pipeline.cpp
    inc/Pipeline.h
    src/PipelineStage.cpp
    inc/PipelineStage.h
    inc/PipelineStageNameConverter.h
    inc/ProducerAndConsumerStage.h
    inc/ProducerStage.h
    inc/StageConnection.h
    inc/StageTask.h
    src/SteadyClock.cpp
    inc/SteadyClock.h
    inc/TaskRetrieveStrategy.h
    inc/PipelineException.h)

target_include_directories(pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

if(MSVC)
    target_link_libraries(pipeline PUBLIC LibCDS::cds)
else()
    target_link_libraries(pipeline PUBLIC LibCDS::cds-s)
endif()

