add_executable(activity_server
               activity_server.cpp
               activity.cpp
               activity_db.cpp
               activity_log.cpp)

target_link_libraries(activity_server
                      activity_proto
                      parser)