add_library(DiGO-engine DistGOServer.cc)
target_include_directories(DiGO-engine PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(DiGO-engine PRIVATE ${PROJECT_BINARY_DIR})


add_subdirectory(proto)

# target_link_libraries(DiGO-engine INTERFACE gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts)
target_link_libraries(DiGO-engine PUBLIC proto_lib)