add_library(activity STATIC
            activity.cpp
            track_point.cpp)

target_link_libraries(activity
                      activitylib)

set_target_properties(activity PROPERTIES
                      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")
