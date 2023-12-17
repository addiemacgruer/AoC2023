#include "point.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <limits>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

auto up = Point{0, -1};
auto down = Point{0, 1};
auto left = Point{-1, 0};
auto right = Point{1, 0};

auto dirs = std::vector<Point>{right, down, left, up};

using Map = std::unordered_map<Point, int>;

struct Parse {
  Map map{};
  int width;
  int height;
};

auto parse(const std::string &filename) {
  auto rval = Parse{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  auto y = 0;
  while (std::getline(input_handle, line)) {
    rval.width = std::max(rval.width, int(line.size()));
    for (auto x = 0; x < rval.width; ++x)
      rval.map.insert({Point{x, y}, line.at(x) - '0'});
    ++y;
  }
  rval.height = y;
  return rval;
}

struct Walk {
  int heat;
  Point pos;
  int dir;
};

auto operator<(const Walk &a, const Walk &b) {
  return a.heat >= b.heat;
}

auto try_next(const Parse &input, int shortest, int longest) {
  auto queue = std::priority_queue<Walk>{};
  auto been = std::unordered_map<Point, int>{};
  queue.emplace(0, Point{0, 0}, -1);
  while (!queue.empty()) {
    auto walk = queue.top();
    queue.pop();

    if (walk.pos.x == input.width - 1 && walk.pos.y == input.height - 1)
      return walk.heat;
    if (been[walk.pos] & (1 << walk.dir))
      continue;
    been[walk.pos] |= (1 << walk.dir);

    for (auto d = 0; d < int(dirs.size()); ++d) {
      if (d != -1 && (d == walk.dir || (d + 2) % 4 == walk.dir))
        continue;
      auto prospective = walk.pos;
      auto p_heat = walk.heat;
      for (auto step = 1; step <= longest; ++step) {
        prospective = prospective + dirs.at(d);
        if (prospective.x < 0 || prospective.y < 0 || prospective.x >= input.width ||
            prospective.y >= input.height)
          break;
        p_heat += input.map.at(prospective);
        if (step >= shortest)
          queue.emplace(p_heat, prospective, d);
      }
    }
  }
  throw std::runtime_error{"lost"};
}

auto part1(const Parse &input) {
  return try_next(input, 1, 3);
}

auto part2(const Parse &input) {
  return try_next(input, 4, 10);
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/17.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input); // 694
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << part2(input); // 829
}
