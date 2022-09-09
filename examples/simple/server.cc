#include <vector>
#include <algorithm>
#include <random>

#include "digo/engine/DistGeneticOptimizer.hpp"
#include "digo/ops_lib/permutation/Crossing.hpp"
#include "digo/ops_lib/permutation/Mutation.hpp"
#include "digo/ops_lib/Selection.hpp"

#include "common.hpp"

int main(int argc, char** argv) {
  auto port = "50051";
  if (argc > 1) {
    port = argv[1];
  }

  std::vector<float> cost_matrix(N*N);
  std::random_device rd{};
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(0.f, 1.f);
  std::generate(cost_matrix.begin(), cost_matrix.end(), [&]() { return dist(gen); });

  auto server = digo::CreateDiGOServer<std::string, digo::PermPopulation>(std::string("0.0.0.0:") + port,
                                                                         TSPEval(N, cost_matrix),
                                                                         digo::CrossPermuts(rd()),
                                                                         digo::RandomSwap(rd(), 0.08),
                                                                         digo::Roulette(rd(), 1, 0.5, 2));
  server.RunAndWait();
  return 0;
}