#include "digo/engine/proto/service.grpc.pb.h"

#include "digo/engine/GOImpl.hpp"

#pragma once

using ::grpc::Status;
using ::grpc::ServerContext;

namespace digo {

template <typename T, typename Population, typename EvalFunc, typename CrossOp, typename MutOp, typename SelectOp>
class GeneticOptimizerService final : public GeneticOptimizer::Service {
 public:
  GeneticOptimizerService(const EvalFunc &eval_func, const CrossOp &cross_op, const MutOp &mut_op, const SelectOp &select_op)
  : impl_(eval_func, cross_op, mut_op, select_op) {}

  Status Optimize(ServerContext *context, const OptRequest *request, OptResponse *response) override {
    auto population = UnpackPopulation(request);
    impl_.Optimize(population, request->intermediate_size(), request->n_iters());
    auto results = impl_.Results(request->final_size());
    PackResultsToRepsonse(results, response);
    return Status::OK;
  }

private:
  static void PackResultsToRepsonse(const OptimizationResult<T> &results, OptResponse *response) {
    Population population;
    *population.mutable_elements() = {results.elements().begin(), results.elements().end()};
    response->mutable_population()->PackFrom(population);
    *response->mutable_fitness() = {results.fitness().begin(), results.fitness().end()};
  }

  static std::vector<T> UnpackPopulation(const OptRequest *request) {
    Population population;
    request->population().UnpackTo(&population);
    return {population.elements().begin(), population.elements().end()};
  }

  GOImpl<T, EvalFunc, CrossOp, MutOp, SelectOp> impl_;
};

}  // namespace digo
