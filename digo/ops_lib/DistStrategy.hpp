#include <algorithm>
#include <vector>
#include <tuple>

#pragma once

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
    for (int i = 0; i < std::min(selection_size_, rank.size()); ++i) {
      selected.push_back(results[std::get<1>(rank[i])].elements()[std::get<2>(rank[i])]);
      selected_f.push_back(std::get<0>(rank[i]));
    }
    return {selected, selected_f};
  }

  uint64_t selection_size_;
};

} // namespace digo
