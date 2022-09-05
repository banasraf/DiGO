add_library(DiGO-engine INTERFACE)
target_include_directories(DiGO-engine INTERFACE .)

add_subdirectory(proto)

# target_link_libraries(DiGO-engine INTERFACE gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts)
target_link_libraries(DiGO-engine INTERFACE proto_lib)