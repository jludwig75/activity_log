add_executable(server_integration_test
                server_integration_test.cpp)

target_link_libraries(server_integration_test
                      activitydb
                      catch2_main
                      parser
                      pthread)

catch_discover_tests(server_integration_test)
