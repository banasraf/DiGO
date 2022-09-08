#include "digo/engine/GeneticOptimizerService.hpp"

#pragma once

namespace digo {

class DistGeneticOptimizerServer {
 public:
  DistGeneticOptimizerServer(std::unique_ptr<GeneticOptimizer::Service> service, std::string address)
  : service_(std::move(service))
  , address_(std::move(address)) {}

  void RunAndWait();

 private:
  std::unique_ptr<GeneticOptimizer::Service> service_;
  std::string address_;
};

}  // namespace digo
