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

catch_discover_tests(file_utils_test)


### parser test
add_executable(parser_test
                parser_test.cpp)

target_link_libraries(parser_test
                        catch2_main
                        parser)

catch_discover_tests(parser_test)


### gpxparser test
add_executable(gpxparser_test
                gpxparser_test.cpp)

target_link_libraries(gpxparser_test
                        catch2_main
                        parser)

catch_discover_tests(gpxparser_test)


### gpxfile test
add_executable(gpxfile_test
                gpxfile_test.cpp)

target_link_libraries(gpxfile_test
                        catch2_main
                        parser
                        pthread)

catch_discover_tests(gpxfile_test)
