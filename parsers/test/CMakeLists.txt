add_executable(parse_activity
               parse_activity.cpp)

# target_include_directories(parse_activity
#                            PRIVATE ../parsers)

target_link_libraries(parse_activity
                      parser
                      pthread)


### file_utils test
add_executable(file_utils_test
                file_utils_test.cpp
                ../file_utils.cpp)

target_link_libraries(file_utils_test
                        catch2_main)


### parser test
add_executable(parser_test
                parser_test.cpp)

target_link_libraries(parser_test
                        catch2_main
                        parser)


### gpxparser test
add_executable(gpxparser_test
                gpxparser_test.cpp)

target_link_libraries(gpxparser_test
                        catch2_main
                        parser)