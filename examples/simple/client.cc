#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <google/protobuf/any.h>

#include "digo/engine/DistGeneticOptimizer.hpp"

namespace digo {

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<grpc::Channel> channel)
      : stub_(GeneticOptimizer::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

  template <typename T>
  std::string SayAny(const T &any) {
    // AnyReq request;
    // request.mutable_details()->set_value(any);
    // HelloReply reply;

    // // Context for the client. It could be used to convey extra information to
    // // the server and/or tweak certain RPC behaviors.
    // grpc::ClientContext context;

    // // The actual RPC.
    // Status status = stub_->SayAny(&context, request, &reply);

    // if (status.ok()) {
    //   return reply.message();
    // } else {
    //   std::cout << status.error_code() << ": " << status.error_message()
    //             << std::endl;
    //   return "RPC failed";
    // }
  }

  std::pair<std::vector<int64_t>, std::vector<float>> Optimize(const std::vector<int64_t> &population) {
    grpc::ClientContext context;
    OptRequest request;
    request.set_final_size(population.size());
    request.set_intermediate_size(population.size());
    request.set_n_iters(1);
    IntPopulation ipopulation;
    for (auto elem: population) {
      ipopulation.add_elements(elem);
    }
    request.mutable_population()->PackFrom(ipopulation);

    OptResponse response;
    auto status = stub_->Optimize(&context, request, &response);
    if (status.ok()) {
      IntPopulation irpop;
      response.population().UnpackTo(&irpop);
      std::vector<int64_t> res_pop(irpop.elements().begin(), irpop.elements().end());
      std::vector<float> res_fitness(response.fitness().begin(), response.fitness().end());
      return {res_pop, res_fitness};
    } else {
      std::cout << "error" << std::endl;
      return {};
    }
  }

 private:
  std::unique_ptr<GeneticOptimizer::Stub> stub_;
};

}

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  std::string target_str;
  std::string arg_str("--target");
  if (argc > 1) {
    std::string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target=" << std::endl;
        return 0;
      }
    } else {
      std::cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }
  digo::GreeterClient greeter(grpc::CreateChannel(
      target_str, grpc::InsecureChannelCredentials()));
  // std::string user("Rafal");
  // std::string reply = greeter.SayHello(user);
  // std::cout << "Greeter received: " << reply << std::endl;
  // digo::HelloRequest request;
  // request.set_name(user);
  // greeter.SayAny(request);
  auto result = greeter.Optimize(std::vector<int64_t>{1, 2, 3, 4, 5});
  for (auto r: result.first) {
    std::cout << r << " ";
  }
  std::cout << std::endl;
  for (auto r: result.second) {
    std::cout << r << " ";
  }
  std::cout << std::endl;

  return 0;
}