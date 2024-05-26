add_library(pipeline_stage_observer
    inc/PipelineStageObserver.h
    inc/PipelineStageFactory.h
    inc/PipelineStageObserverException.h
    inc/StageRepresentation.h
    
    src/PipelineStageObserver.cpp)

target_include_directories(pipeline_stage_observer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(pipeline_stage_observer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../pipeline/inc)
target_include_directories(pipeline_stage_observer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../pipeline_sample/inc)

