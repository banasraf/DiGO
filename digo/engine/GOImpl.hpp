#include <vector>

#include "digo/common/error.hpp"
#include "digo/common/types.h"

#pragma once

namespace digo {

template <typename T>
struct OptimizationResult {
  OptimizationResult(std::vector<T> elements, std::vector<fitness_t> fitness)
  : elements_(std::move(elements))
  , fitness_(std::move(fitness)) {
    DIGO_ENFORCE(elements_.size() == fitness_.size(),
                 "Size of the population and fitness vector mismatched");
  }

  const auto &elements() const {
    return elements_;
  }

  const auto &fitness() const {
    return fitness_;
  }

 private:
  std::vector<T> elements_;
  std::vector<fitness_t> fitness_;
};

template <typename T, typename EvalFunc, typename CrossOp, typename MutOp, typename SelectOp>
class GOImpl {
 public:
  GOImpl(const EvalFunc &eval_func, const CrossOp &cross_op, const SelectOp &select_op)
  : eval_func_(eval_func)
  , cross_op_(cross_op)
  , select_op_(select_op) {}

  void Optimize(std::vector<T> population,
                uint64_t final_size, uint64_t intermediate_size,
                uint64_t n_iters) {
    population_ = std::move(population);
    CalcFitness();
    for (uint64_t i = 1; i < n_iters; ++i) {
      OptStep(intermediate_size);
    }
    OptStep(final_size);
  }

  OptimizationResult<T> Results() const {
    return OptimizationResult(population_, fitness_);
  }

  const std::vector<fitness_t> &fitness() const {
    return fitness_;
  }

 private:
  void CalcFitness() {
    fitness_.clear();
    fitness_.reserve(population_.size());
    for (const auto &elem: population_) {
      fitness_.push_back(eval_func_.Eval(elem));
    }
  }

  void OptStep(uint64_t next_size) {
    auto selection = select_op_.Select(fitness_, next_size);
    DIGO_ENFORCE(selection.winners.size() + selection.matches.size() == next_size,
                 "Invalid selection.");
    tmp_population_.clear();
    tmp_population_.reserve(next_size);
    for (auto elem_idx: selection.winners) {
      tmp_population_.push_back(population_[elem_idx]);
    }
    ProduceNewPopulation(selection.matches);
    std::swap(population_, tmp_population_);
    CalcFitness();
  }

  void ProduceNewPopulation(const std::vector<match_t> &matches) {
    tmp_population_.reserve(tmp_population_.size() + matches.size());
    for (auto match: matches) {
      auto new_elem = cross_op_.Cross(population_[match.first], population_[match.second]);
      tmp_population_.push_back(new_elem);
    }
  }

  std::vector<T> population_;
  std::vector<T> tmp_population_;
  std::vector<fitness_t> fitness_;
  EvalFunc eval_func_;
  CrossOp cross_op_;
  SelectOp select_op_;
};

} // namespace digo