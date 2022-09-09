add_executable(simple-client client.cc)
target_link_libraries(simple-client DiGO)

add_executable(simple-server server.cc)
target_link_libraries(simple-server DiGO)