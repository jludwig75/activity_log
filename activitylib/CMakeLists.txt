add_library(activitylib STATIC
            threadfile.cpp)

target_link_libraries(activitylib)

set_target_properties(activitylib PROPERTIES
                      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")
