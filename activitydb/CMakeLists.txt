add_library(activitydb STATIC
            activity_db.cpp)

target_include_directories(activitydb
                PRIVATE ${LIBMONGOCXX_INCLUDE_DIRS})

target_link_libraries(activitydb
                      activity
                      activitylib
                      ${LIBMONGOCXX_LIBRARIES})

set_target_properties(activitydb PROPERTIES
                      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")
