#include "parsing.h"
#include <algorithm>
#include <boost/log/trivial.hpp>
#include <fstream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

struct ScratchCard {
  int game;
  std::unordered_set<int> winners{};
  std::unordered_set<int> values{};
};

using ScratchCards = std::vector<ScratchCard>;

auto parse(const std::string &filename) {
  auto rval = ScratchCards{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  auto s = std::string{};
  auto i = int{};
  auto ss = std::stringstream{};
  while (getline(input_handle, ss)) {
    auto card = ScratchCard{};
    ss >> s >> card.game >> s;
    while (ss >> s) {
      if (s == "|")
        break;
      card.winners.insert(std::stoi(s));
    }
    while (ss >> i && i)
      card.values.insert(i);
    rval.push_back(std::move(card));
  }
  return rval;
}

auto match_count(const ScratchCard &card) {
  return std::count_if(card.winners.begin(), card.winners.end(), [&card](auto win) {
    return card.values.contains(win);
  });
}

auto part1(const ScratchCards &input) {
  return std::accumulate(input.begin(), input.end(), 0L, [](auto count, auto &card) {
    auto matched = match_count(card);
    return matched ? count + (1 << (matched - 1)) : count;
  });
}

auto part2(const ScratchCards &input) {
  auto copies = std::unordered_map<int, size_t>{};
  for (auto &c : input)
    copies.insert({c.game, 1});
  for (auto &card : input) {
    auto score = match_count(card);
    auto copy = copies.at(card.game);
    for (auto j = card.game + 1; j <= card.game + score; ++j)
      copies.at(j) += copy;
  }
  return std::accumulate(copies.begin(), copies.end(), 0L, [](auto i, auto &copy) {
    return i + copy.second;
  });
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/04.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input); // 27845
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << part2(input); // 9496801
}
