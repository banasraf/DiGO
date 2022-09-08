#include <iostream>
#include <memory>
#include <string>
#include <algorithm>

#include <grpcpp/grpcpp.h>
#include <google/protobuf/any.h>

#include "digo/engine/DistGeneticOptimizer.hpp"

namespace digo {

template <typename T>
struct MultiplyAndSelect {
  MultiplyAndSelect(uint64_t selection_size): selection_size_(selection_size) {}

  std::vector<std::vector<T>> Distribute(const std::vector<T> &population, uint64_t parts) {
    return std::vector<std::vector<T>>(parts, population);
  }

  OptimizationResult<T> Merge(const std::vector<OptimizationResult<T>> &results) {
    std::vector<std::tuple<fitness_t, idx_t, idx_t>> rank;
    for (idx_t r = 0; r < results.size(); ++r) {
      for (idx_t i = 0; i < results[r].fitness().size(); ++i) {
        rank.push_back(std::make_tuple(results[r].fitness()[i], r, i));
      }
    }
    std::sort(rank.begin(), rank.end(), std::greater());

    std::vector<T> selected;
    selected.reserve(selection_size_);
    std::vector<fitness_t> selected_f;
    selected_f.reserve(selection_size_);
    for (int i = 0; i < selection_size_; ++i) {
      selected.push_back(results[std::get<1>(rank[i])].elements()[std::get<2>(rank[i])]);
      selected_f.push_back(std::get<0>(rank[i]));
    }
    return {selected, selected_f};
  }

  uint64_t selection_size_;
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
  // digo::GreeterClient greeter(grpc::CreateChannel(
  //     target_str, grpc::InsecureChannelCredentials()));
  // std::string user("Rafal");
  // std::string reply = greeter.SayHello(user);
  // std::cout << "Greeter received: " << reply << std::endl;
  // digo::HelloRequest request;
  // request.set_name(user);
  // greeter.SayAny(request);
  // auto result = greeter.Optimize(std::vector<int64_t>{1, 2, 3, 4, 5});
  auto client = digo::CreateDiGOClient<int64_t, digo::IntPopulation>(std::vector<std::string>{target_str, "localhost:50052"}, digo::MultiplyAndSelect<int64_t>(4));
  auto result = client.Optimize(3, 5, 10, std::vector<int64_t>{1, 2, 3, 4, 5});
  for (auto r: result.elements()) {
    std::cout << r << " ";
  }
  std::cout << std::endl;
  for (auto r: result.fitness()) {
    std::cout << r << " ";
  }
  std::cout << std::endl;

  return 0;
}