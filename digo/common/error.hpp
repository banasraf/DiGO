#include <stdexcept>

#pragma once

namespace digo {

class DiGOError : public std::runtime_error {
 public:
  DiGOError(const std::string &msg): std::runtime_error(msg) {}
};

#define DIGO_ENFORCE(cond, msg) \
while (!(cond)) {                  \
  throw DiGOError(std::string(msg)); break;   \
};

} // namespace digo
