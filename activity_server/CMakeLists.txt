set (CMAKE_SHARED_LINKER_FLAGS " -Wl,-rpath,/usr/local/lib")
set (CMAKE_STATIC_LINKER_FLAGS " -Wl,-rpath,/usr/local/lib")

add_executable(activity_server
               activity_server.cpp
               activity.cpp
               activity_db.cpp
               activity_log.cpp)

target_include_directories(activity_server 
                PRIVATE ${LIBMONGOCXX_INCLUDE_DIRS})   

target_link_libraries(activity_server
                      activity_proto
                      activitylib
                      parser
                      ${LIBMONGOCXX_LIBRARIES})