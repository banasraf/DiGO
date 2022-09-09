#include <grpcpp/grpcpp.h>
#include <google/protobuf/any.h>

#include "digo/engine/proto/service.grpc.pb.h"
#include "digo/engine/GOImpl.hpp"

#pragma once

namespace digo {

template <typename T, typename Population, typename DistStrategy>
class DistGeneticOptimizerClient {
 public:
  DistGeneticOptimizerClient(const std::vector<std::string> &servers, const DistStrategy &dist_strategy)
  : dist_strategy_(dist_strategy) {
    servers_.reserve(servers.size());
    for (auto address: servers) {
      auto channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
      servers_.emplace_back(GeneticOptimizer::NewStub(std::move(channel)));
    }
  }

  OptimizationResult<T> Optimize(uint64_t final_size, uint64_t intermediate_size,
                                 uint64_t n_iters,
                                 const std::vector<T> &population) {
    std::vector<OptRequest> requests(servers_.size());
    std::vector<std::vector<T>> dist_pops = dist_strategy_.Distribute(population, servers_.size());
    std::vector<std::unique_ptr<grpc::ClientAsyncResponseReader<OptResponse>>> rpcs;
    std::vector<grpc::ClientContext> contexts(servers_.size());
    for (size_t i = 0; i < servers_.size(); ++i) {
      PackRequest(final_size, intermediate_size, n_iters, population, &requests[i]);
      rpcs.emplace_back(
        servers_[i]->PrepareAsyncOptimize(&contexts[i], requests[i], &completion_queue_)
      );
      rpcs.back()->StartCall();
    }
    std::vector<OptResponse> responses(servers_.size());
    std::vector<grpc::Status> statuses(servers_.size());
    for (size_t i = 0; i < servers_.size(); ++i) {
      rpcs[i]->Finish(&responses[i], &statuses[i], reinterpret_cast<void*>(i));
    }
    std::vector<OptimizationResult<T>> results(servers_.size());
    for (size_t i = 0; i < servers_.size(); ++i) {
      void *tag;
      bool ok;
      completion_queue_.Next(&tag, &ok);
      if (!ok) {
        throw DiGOError("Error in async request.");
      }
      auto id = reinterpret_cast<size_t>(tag);
      DIGO_ENFORCE(id < servers_.size(), "Incorrect response form a server.");
      DIGO_ENFORCE(statuses[id].ok(), "RPC error.")
      results[id] = UnpackResponse(responses[id]);
    }
    return dist_strategy_.Merge(results);
  }

 private:
  static void PackRequest(uint64_t final_size, uint64_t intermediate_size,
                          uint64_t n_iters,
                          const std::vector<T> &population,
                          OptRequest *request) {
    request->set_final_size(final_size);
    request->set_intermediate_size(intermediate_size);
    request->set_n_iters(n_iters);
    Population rpopulation;
    for (auto elem: population) {
      rpopulation.add_elements(elem);
    }
    request->mutable_population()->PackFrom(rpopulation);
  }

  static OptimizationResult<T> UnpackResponse(const OptResponse &response) {
    Population rpop;
    response.population().UnpackTo(&rpop);
    std::vector<T> res_pop(rpop.elements().begin(), rpop.elements().end());
    std::vector<fitness_t> res_fitness(response.fitness().begin(), response.fitness().end());
    return {std::move(res_pop), std::move(res_fitness)};
  }

  DistStrategy dist_strategy_;
  std::vector<std::unique_ptr<GeneticOptimizer::Stub>> servers_{};
  grpc::CompletionQueue completion_queue_;
};

} // namespace digo
