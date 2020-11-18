add_library(activitylib STATIC
            fileutils.cpp
            threadfile.cpp)

set_target_properties(activitylib PROPERTIES
                      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")
