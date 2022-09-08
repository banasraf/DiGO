#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "digo/engine/DistGeneticOptimizer.hpp"


namespace digo {

struct EvalFunc {
  float Eval(int a) {
    return static_cast<float>(a);
  }
};


struct SelectOp {
  Selection Select(const std::vector<float> &fitness, uint64_t next_gen_size) {
    std::vector<idx_t> winners(next_gen_size);
    for (idx_t i = 0; i < next_gen_size; ++i) winners[i] = i;
    return Selection {winners, std::vector<match_t>{}};
  }
};

struct CrossOp {
  int Cross(int a, int b) {
    return (a + b) / 2;
  }
};

struct MutOp {
  int Mut(int a) { return a; }
};

}  // namespace digo

int main(int argc, char** argv) {
  auto port = "50051";
  if (argc > 1) {
    port = argv[1];
  }
  auto server = digo::CreateDiGOServer<int, digo::IntPopulation>(std::string("0.0.0.0:") + port,
                                                                 digo::EvalFunc{},
                                                                 digo::CrossOp{},
                                                                 digo::MutOp{},
                                                                 digo::SelectOp{});
  server.RunAndWait();
  return 0;
}