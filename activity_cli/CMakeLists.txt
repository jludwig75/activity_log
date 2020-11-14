add_executable(activity
               activity_cli.cpp
               activity.cpp
               activity_log.cpp)

target_link_libraries(activity
                      activity_proto
                      activitylib)