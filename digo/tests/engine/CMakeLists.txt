add_executable(engine_tests
               DistGeneticOptimizerTests.cpp)
target_link_libraries(engine_tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)