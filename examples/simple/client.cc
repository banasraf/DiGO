#include <iostream>
#include <memory>
#include <string>
#include <algorithm>
#include <random>

#include "digo/engine/DistGeneticOptimizer.hpp"
#include "digo/ops_lib/DistStrategy.hpp"

#include "common.hpp"

std::vector<std::string> RandomPopulation(size_t size) {
  std::random_device rd{};
  std::mt19937 gen(rd());

  std::vector<std::string> population;
  for (size_t i = 0; i < size; ++i) {
    std::string perm;
    for (char c = '0'; c < '0' + N; ++c) {
      perm.push_back(c);
    }
    std::shuffle(perm.begin(), perm.end(), gen);
    population.push_back(perm);
  }
  return population;
}

int main(int argc, char** argv) {
  std::vector<std::string> servers;
  for (int i = 1; i < argc; ++i) {
    servers.emplace_back(argv[i]);
  }

  auto initial_population = RandomPopulation(1000);

  auto client = digo::CreateDiGOClient<std::string, digo::PermPopulation>(servers, digo::MultiplyAndSelect<std::string>(100));
  uint64_t epoch_iters = 10;
  uint64_t epochs = 100;
  auto population = initial_population;
  for (uint64_t i = 0; i < epochs; ++i) {
    auto result = client.Optimize(population.size() / 2, population.size() * 2, epoch_iters, population);
    std::cout << "epoch " << i+1 << " / " << epochs << "; best fitness: ";
    std::cout << *std::max_element(result.fitness().begin(), result.fitness().end()) << std::endl;
    population = result.elements();
  }

  return 0;
}