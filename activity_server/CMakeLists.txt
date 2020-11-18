add_executable(activity_server
               activity_server.cpp
               activity_log.cpp)

target_link_libraries(activity_server
                      activity_proto
                      activitydb
                      parser)

add_subdirectory(test)