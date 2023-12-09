#include "point.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using Parse = std::vector<std::string>;

auto parse(const std::string &filename) {
  auto rval = Parse{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  while (std::getline(input_handle, line))
    rval.push_back(line);
  return rval;
}

struct PartPosition {
  Point pos;
  int part;
};

constexpr auto is_digit(char c) {
  return c >= '0' && c <= '9';
}

constexpr auto is_symbol(char c) {
  return !is_digit(c) && c != '.';
}

auto has_part_symbol(const Parse &input, const Point &a, const Point &b, int xmax, int ymax) {
  for (auto y = std::max(0, a.y - 1); y <= std::min(ymax - 1, a.y + 1); ++y)
    for (auto x = std::max(0, a.x - 1); x <= std::min(xmax - 1, b.x + 1); ++x)
      if (is_symbol(input.at(y).at(x)))
        return PartPosition{{x, y}, int(input.at(y).at(x))};
  return PartPosition{{}, 0};
}

auto solve(const Parse &input) {
  auto rval1 = 0;
  auto rval2 = 0;
  auto ymax = int(input.size());
  auto xmax = int(input.at(0).size());
  auto gears = std::map<Point, int>{};

  for (auto y = 0; y < ymax; ++y) {
    for (auto x = 0; x < xmax; ++x) {
      if (!is_digit(input.at(y).at(x)))
        continue;
      auto first = Point{x, y};
      auto val = input.at(y).at(x) - '0';
      while (true) {
        ++x;
        if (x == xmax || !is_digit(input.at(y).at(x)))
          break;
        val = val * 10 + input.at(y).at(x) - '0';
      }
      auto last = Point{x - 1, y};
      auto part = has_part_symbol(input, first, last, xmax, ymax);
      if (part.part == '*') {
        rval1 += val;
        if (auto i = gears.find(part.pos); i != gears.end()) {
          rval2 += val * i->second;
        } else {
          gears.insert({part.pos, val});
        }
      } else if (part.part) {
        rval1 += val;
      }
    }
  }
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << rval1 << " Part 2: " << rval2;
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/03.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  solve(input);
}
