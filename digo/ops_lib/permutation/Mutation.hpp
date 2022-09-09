#include <string>
#include <algorithm>
#include <random>

#include "digo/common/error.hpp"

#pragma once

namespace digo {

class RandomSwap {
 public:
  RandomSwap(size_t seed, float probability): prob_(probability), gen(seed) {}

  std::string Mut(const std::string &elem) {
    DIGO_ENFORCE(*std::min_element(elem.begin(), elem.end()) == '0', "Ill-formed permutation");
    DIGO_ENFORCE(*std::max_element(elem.begin(), elem.end()) == '0' + elem.size()-1, "Ill-formed permutation");

    std::uniform_real_distribution<float> fdist(0.f, 1.f);
    if (fdist(gen) > prob_) {
      return elem;
    }

    std::uniform_int_distribution<size_t> dist(0, elem.size() - 1);
    auto a = dist(gen);
    auto b = dist(gen);
    while (b == a) {
      b = dist(gen);
    }
    std::string result = elem;
    std::swap(result[a], result[b]);
    return result;
  }

 private:
  float prob_;
  std::mt19937 gen;
};

} // namespace digo
