add_library(parser STATIC
            parser.cpp
            gpxparser.cpp)

target_link_libraries(parser
                      xerces-c)

set_target_properties(parser PROPERTIES
                      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(test)
