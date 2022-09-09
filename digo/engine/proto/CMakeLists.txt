# Adapted from https://github.com/advra/grpc-vcpkg-boilerplate-example/blob/main/protos/CMakeLists.txt

find_program(PROTOC NAMES protoc)
if (NOT EXISTS ${PROTOC})
  message(FATAL_ERROR "The protoc program was not found")
endif()
message(STATUS "Found protoc program: " ${PROTOC})

find_program(GRPC_PLUGIN NAMES "grpc_cpp_plugin")
if (NOT EXISTS ${GRPC_PLUGIN})
  message(WARNING "The grpc_cpp_plugin plugin was not found, \
                   the gRPC classes are not being generated")
else(EXISTS ${GRPC_PLUGIN})
  message(STATUS "Found grpc_cpp_plugin : " ${GRPC_PLUGIN})
endif()

add_library(proto_lib service.proto)

protobuf_generate(TARGET proto_lib LANGUAGE cpp)
protobuf_generate(TARGET proto_lib LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${GRPC_PLUGIN}")

target_link_libraries(proto_lib PUBLIC gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts re2::re2 c-ares::cares)