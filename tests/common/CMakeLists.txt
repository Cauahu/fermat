add_executable(cmdline_test cmdline_test.cc)
target_link_libraries(cmdline_test fermatStatic)

add_executable(log_test log_test.cc)
target_link_libraries(log_test fermatStatic)