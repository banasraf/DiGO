#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "digo/engine/DistGeneticOptimizer.hpp"
#include "digo/engine/GeneticOptimizerService.hpp"



namespace digo {

struct EvalFunc {
  float Eval(int a) {
    return static_cast<float>(a);
  }
};


struct SelectOp {
  Selection Select(const std::vector<float> &fitness, uint64_t next_gen_size) {
    return Selection {std::vector<idx_t>(next_gen_size, 0), std::vector<match_t>{}};
  }
};

struct CrossOp {
  int Cross(int a, int b) {
    return (a + b) / 2;
  }
};


void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GeneticOptimizerService<int, EvalFunc, CrossOp, void, SelectOp, IntPopulation>  service(EvalFunc{}, CrossOp{}, SelectOp{});

  grpc::EnableDefaultHealthCheckService(true);
  // grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

}  // namespace digo

int main(int argc, char** argv) {
  digo::RunServer();

  return 0;
}