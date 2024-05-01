add_library(pipeline inc/stage.h inc/producing_stage.h inc/consuming_stage.h inc/pipeline.h inc/mpmc_cycle_queue.h src/stub.cpp src/stage.cpp src/pipeline.cpp)

target_include_directories(pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(pipeline PUBLIC LibCDS::cds)

