#include <algorithm>
#include <cmath>
#include <random>

#include "digo/common/types.h"

#pragma once

namespace digo {

struct Roulette {
  /**
   * @brief Selection operator that passes through num_winners of best elements
   * and selectes the matches with probability proportional to fitness.
   * Probability of selecting element with fitness `f` is directly proportional to:
   * (f / max_fitness)^e
   * Only elements with fitness higher than fit_treshold*max_fitness are considered for selection.
   * fit_treshold must be in range (0, 1)
   */
  Roulette(size_t seed, uint64_t num_winners, float fit_treshhold, float e)
  : num_winners_(num_winners)
  , fit_treshhold_(fit_treshhold)
  , exp_(e)
  , rgen_(seed) {}

  Selection Select(const std::vector<fitness_t> &fitness, uint64_t next_gen_size) {
    fitness_t max_fitness = *std::max_element(fitness.begin(), fitness.end());
    std::vector<float> weights;
    weights.push_back(0);
    std::vector<idx_t> filtered;
    for (idx_t i = 0; i < fitness.size(); ++i) {
      if (fitness[i] > fit_treshhold_ * max_fitness) {
        filtered.emplace_back(i);
        float weight = std::pow((fitness[i] / max_fitness), exp_);
        weights.push_back(weight + weights.back());
      }
    }
    std::vector<std::pair<idx_t, idx_t>> matches;
    std::uniform_real_distribution<> dist(0.0f, weights.back());
    for (uint64_t i = num_winners_; i < next_gen_size; ++i) {
      auto id1 = filtered[RandomSelect(dist, weights)];
      auto id2 = filtered[RandomSelect(dist, weights)];
      matches.emplace_back(id1, id2);
    }
    return {SelectWinners(fitness), matches};
  }

 private:
  std::vector<idx_t> SelectWinners(const std::vector<fitness_t> &fitness) {
    std::vector<std::pair<fitness_t, idx_t>> rank;
    for (idx_t i = 0; i < fitness.size(); ++i) {
      rank.emplace_back(fitness[i], i);
    }
    std::sort(rank.begin(), rank.end(), std::greater());
    std::vector<idx_t> winners;
    for (uint64_t i = 0; i < num_winners_; ++i) {
      winners.push_back(rank[i].first);
    }
    return winners;
  }

  idx_t RandomSelect(std::uniform_real_distribution<> &dist, const std::vector<float> weights) {
    auto shot = dist(rgen_);
    auto lb = std::lower_bound(weights.begin(), weights.end(), shot);
    return lb - weights.begin() - 1;
  }

  uint64_t num_winners_;
  float fit_treshhold_;
  float exp_;
  std::mt19937 rgen_;
};

}  // namespace digo