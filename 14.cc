#include <algorithm>
#include <boost/log/trivial.hpp>
#include <fstream>
#include <iterator>
#include <numeric>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "point.h"

namespace {

struct Dish {
  std::unordered_set<Point> round{};
  std::unordered_set<Point> square{};
  int width;
  int height;
};

using Parse = Dish;

auto parse(const std::string &filename) {
  auto rval = Parse{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  auto y = 0;
  while (std::getline(input_handle, line)) {
    rval.width = std::max(rval.width, int(line.size()));
    for (auto x = 0; x < int(line.size()); ++x) {
      if (line.at(x) == 'O')
        rval.round.insert({x, y});
      else if (line.at(x) == '#')
        rval.square.insert({x, y});
    }
    ++y;
  }
  rval.height = y;
  return rval;
}

auto roll(Dish d, const Point &dir) {
  auto rollers = std::vector<Point>{d.round.begin(), d.round.end()};
  if (dir == Point{0, -1} || dir == Point{-1, 0}) {
    std::sort(rollers.begin(), rollers.end(), [&](const Point &a, const Point &b) {
      return a.y == b.y ? a.x < b.x : a.y < b.y;
    });
  } else {
    std::sort(rollers.begin(), rollers.end(), [&](const Point &a, const Point &b) {
      return a.y == b.y ? a.x > b.x : a.y > b.y;
    });
  }
  auto next_round = decltype(d.round){};
  for (auto &roller : rollers) {
    auto r = roller;
    while (true) {
      auto prospective = r + dir;
      if (prospective.y < 0 || prospective.x < 0 || prospective.x >= d.width ||
          prospective.y >= d.height) {
        next_round.insert(r);
        break;
      }
      if (next_round.contains(prospective) || d.square.contains(prospective)) {
        next_round.insert(r);
        break;
      }
      r = prospective;
    }
  }
  std::swap(d.round, next_round);
  return d;
}

auto load(const Dish &d) {
  return std::accumulate(d.round.begin(), d.round.end(), 0L, [&d](size_t a, const Point &p) {
    return a + d.height - p.y;
  });
}

auto dodgy_hash(const Dish &d) {
  return std::accumulate(d.round.begin(), d.round.end(), 13L, [](size_t a, const Point &p) {
    return 7 * a + std::hash<Point>()(p);
  });
}

auto part1(const Parse &input) {
  return load(roll(input, {0, -1}));
}

constexpr auto LIMIT = 1000000000;

auto part2(Dish dish) {
  auto loop = std::vector<size_t>{};
  auto cycle = 0;
  while (true) {
    dish = roll(dish, {0, -1});
    dish = roll(dish, {-1, 0});
    dish = roll(dish, {0, 1});
    dish = roll(dish, {1, 0});
    auto nextup = dodgy_hash(dish);
    if (auto find = std::find(loop.begin(), loop.end(), nextup); find != loop.end()) {
      auto skip = 4 * std::distance(find, loop.end());
      while (cycle + skip < LIMIT)
        cycle += skip;
      loop.clear();
    }
    cycle += 4;
    loop.push_back(nextup);
    if (cycle > LIMIT)
      return load(dish);
  }
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/14.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input); // 110779
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << part2(input); // 86069
}
