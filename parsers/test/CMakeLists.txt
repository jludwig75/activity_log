add_executable(parse_activity
               parse_activity.cpp)

# target_include_directories(parse_activity
#                            PRIVATE ../parsers)

target_link_libraries(parse_activity
                      parser
                      pthread)


add_executable(file_utils_test
                file_utils_test.cpp
                ../file_utils.cpp)

target_link_libraries(file_utils_test
                        catch2_main)


add_executable(gpxparser_test
                gpxparser_test.cpp
                ../gpxparser.cpp)

target_link_libraries(gpxparser_test
                        catch2_main
                        parser)