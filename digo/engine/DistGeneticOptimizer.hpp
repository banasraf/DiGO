#include "digo/engine/DistGOServer.hpp"
#include "digo/engine/DistGOClient.hpp"

#pragma once

namespace digo {

template <typename T, typename Population, typename EvalFunc, typename CrossOp, typename MutOp, typename SelectOp>
DistGeneticOptimizerServer CreateDiGOServer(std::string address,
                                            const EvalFunc &eval_func,
                                            const CrossOp &cross_op,
                                            const MutOp &mut_op,
                                            const SelectOp &select_op) {
  using Service_t = GeneticOptimizerService<T, Population, EvalFunc, CrossOp, MutOp, SelectOp>;
  std::unique_ptr<GeneticOptimizer::Service> service;
  service = std::make_unique<Service_t>(eval_func, cross_op, mut_op, select_op);
  return DistGeneticOptimizerServer(std::move(service), std::move(address));
}

template <typename T, typename Population, typename DistStrategy>
auto CreateDiGOClient(const std::vector<std::string> &servers, const DistStrategy &dist_strategy) {
  return DistGeneticOptimizerClient<T, Population, DistStrategy>(servers, dist_strategy);
}

}  // namespace digo
