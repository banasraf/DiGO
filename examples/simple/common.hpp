#include "digo/common/error.hpp"
#include "digo/common/types.h"

#pragma once

struct TSPEval {
  TSPEval(size_t n, const std::vector<float> &cost_matrix)
  : n_cities(n)
  , cost_matrix_(cost_matrix) {
    DIGO_ENFORCE(n*n == cost_matrix.size(), "Mismatch in problem size and matrix size.")
  }

  float Eval(const std::string &perm) {
    size_t previous_city = 0;
    float cost = 0.f;
    for (char c: perm) {
      size_t next_city = static_cast<size_t>(c - '0');
      cost += cost_matrix_[previous_city * n_cities + next_city];
      previous_city = next_city;
    }
    cost += cost_matrix_[previous_city * n_cities + 0];
    return static_cast<float>(n_cities) / cost;
  }

  size_t n_cities;
  std::vector<float> cost_matrix_;
};

const inline size_t N = 50;