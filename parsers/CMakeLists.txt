add_library(parser STATIC
            parser.cpp
            gpxfile.cpp
            gpxparser.cpp
            file_utils.cpp)

target_link_libraries(parser
                      activity
                      pugixml)

set_target_properties(parser PROPERTIES
                      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(test)
