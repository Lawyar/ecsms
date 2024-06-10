enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(common_tests)
add_subdirectory(pipeline_tests)
add_subdirectory(pipeline_registry_tests)
add_subdirectory(database_library_tests)
add_subdirectory(analytics_tests)
