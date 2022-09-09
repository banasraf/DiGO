#include <string>
#include <algorithm>
#include <random>

#include "digo/common/error.hpp"

#pragma once

namespace digo {

class CrossPermuts {
 public:
  /**
   * @brief Cross operator that randomly choses a prefix of new permuataion from `a`
   * and fills the rest of the perumatation in order from the `b`.
   */
  CrossPermuts(size_t seed): gen(seed) {}

  CrossPermuts(const CrossPermuts &) = default;

  std::string Cross(const std::string &a, const std::string &b) {
    DIGO_ENFORCE(a.size() == b.size(), "Only permuatation of equal lenght can be crossed.");
    DIGO_ENFORCE(*std::min_element(a.begin(), a.end()) == '0', "Ill-formed permutation");
    DIGO_ENFORCE(*std::min_element(b.begin(), b.end()) == '0', "Ill-formed permutation");
    DIGO_ENFORCE(*std::max_element(a.begin(), a.end()) == '0' + a.size()-1, "Ill-formed permutation");
    DIGO_ENFORCE(*std::max_element(b.begin(), b.end()) == '0' + b.size()-1, "Ill-formed permutation");
    std::uniform_int_distribution<size_t> dist(0, a.size() - 1);
    auto split_point = dist(gen);
    std::string result(a.substr(0, split_point));
    for (char c: b) {
      if (result.find(c) == std::string::npos) {
        result.push_back(c);
      }
    }
    return result;
  }

 private:
  std::mt19937 gen;
};

} // namespace digo
