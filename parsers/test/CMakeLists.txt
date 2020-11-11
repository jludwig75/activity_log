add_executable(parse_activity
               parse_activity.cpp)

# target_include_directories(parse_activity
#                            PRIVATE ../parsers)

target_link_libraries(parse_activity
                      parser
                      pthread)