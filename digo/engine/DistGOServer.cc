#include "digo/engine/DistGOServer.hpp"

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

namespace digo {

void DistGeneticOptimizerServer::RunAndWait() {
  grpc::EnableDefaultHealthCheckService(true);
  grpc::ServerBuilder builder;
  builder.AddListeningPort(address_, grpc::InsecureServerCredentials());
  builder.RegisterService(service_.get());
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

  server->Wait();
}

}  // namespace digo