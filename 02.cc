#include "parsing.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Subset {
  int red;
  int green;
  int blue;
};

struct Game {
  int id;
  std::vector<Subset> subsets{};
};

auto parse(const std::string &filename) {
  auto rval = std::vector<Game>{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto number = int{};
  auto word = std::string{};
  auto ss = std::stringstream{};
  while (getline(input_handle, ss)) {
    auto game = Game{};
    ss >> word; // game
    ss >> game.id;
    ss >> word; // :
    auto subset = Subset{};
    while (ss) {
      ss >> number; // count
      ss >> word;   // colour

      switch (word[0]) {
      case 'r':
        subset.red = number;
        break;
      case 'g':
        subset.green = number;
        break;
      case 'b':
        subset.blue = number;
        break;
      }

      if (word[word.size() - 1] == ';' || !ss) {
        game.subsets.push_back(subset);
        subset = Subset{};
      }
    }
    rval.push_back(game);
  }
  return rval;
}

auto part1(const std::vector<Game> &input) {
  auto id_sum = 0;
  for (const auto &game : input) {
    for (const auto &subset : game.subsets) {
      if (subset.red > 12)
        goto bad;
      if (subset.green > 13)
        goto bad;
      if (subset.blue > 14)
        goto bad;
    }
    id_sum += game.id;
  bad:;
  }
  return id_sum;
}

auto part2(const std::vector<Game> &input) {
  auto power_sum = 0;
  for (const auto &game : input) {
    auto power = Subset{};
    for (const auto &subset : game.subsets) {
      power.red = std::max(power.red, subset.red);
      power.green = std::max(power.green, subset.green);
      power.blue = std::max(power.blue, subset.blue);
    }
    power_sum += power.red * power.green * power.blue;
  }
  return power_sum;
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/02.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input); // 2632
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << part2(input); // 69629
}
