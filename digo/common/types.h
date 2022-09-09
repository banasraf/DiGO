#include <cstdint>
#include <vector>

#pragma once

namespace digo {

using fitness_t = float;

using idx_t = uint64_t;

using match_t = std::pair<idx_t, idx_t>;

struct Selection {
  std::vector<idx_t> winners;
  std::vector<match_t> matches;
};

}  // namespace digo
