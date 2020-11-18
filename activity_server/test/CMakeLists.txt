set (CMAKE_SHARED_LINKER_FLAGS " -Wl,-rpath,/usr/local/lib")
set (CMAKE_STATIC_LINKER_FLAGS " -Wl,-rpath,/usr/local/lib")

add_executable(server_integration_test
                server_integration_test.cpp
                ../activity_db.cpp)

target_include_directories(server_integration_test
                PRIVATE ${LIBMONGOCXX_INCLUDE_DIRS})   

target_link_libraries(server_integration_test
                      activitylib
                      catch2_main
                      parser
                      pthread
                      ${LIBMONGOCXX_LIBRARIES})

catch_discover_tests(server_integration_test)
