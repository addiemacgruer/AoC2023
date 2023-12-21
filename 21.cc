#include "parsing.h"
#include "point.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

using Garden = std::unordered_set<Point>;

struct Parse {
  Garden garden{};
  Point start;
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
    for (auto x = 0; x < rval.width; ++x) {
      switch (line.at(x)) {
      case 'S':
        rval.start = Point{x, y};
        BOOST_FALLTHROUGH;
      case '.':
        rval.garden.insert({x, y});
        break;
      case '#':
        break;
      default:
        throw std::runtime_error{"?"};
      }
    }
    ++y;
  }
  rval.height = y;
  BOOST_LOG_TRIVIAL(debug) << rval.garden.size() << ' ' << rval.start << " " << rval.width << "x"
                           << rval.height;
  return rval;
}

auto part1(const Parse &input, size_t steps = 6) {
  auto walk = std::unordered_map<Point, size_t>{};
  auto next = std::vector<Point>{};
  next.push_back(input.start);

  auto step = size_t{};
  while (step <= steps) {
    auto nextup = std::vector<Point>{};
    while (!next.empty()) {
      auto n = next.back();
      next.pop_back();
      if (walk.contains(n))
        continue;

      walk.emplace(n, step);
      for (auto &dir : std::vector<Point>{P::up, P::down, P::left, P::right}) {
        auto prospective = n + dir;
        if (!input.garden.contains(prospective))
          continue;
        if (walk.contains(prospective))
          continue;
        nextup.push_back(prospective);
      }
    }
    std::swap(next, nextup);
    ++step;
  }

  auto rval = size_t{};
  for (auto &w : walk)
    if (w.second % 2 == 0)
      ++rval;
  return rval;
}

auto count_in_square(const Parse &input, const Point &entry, long steps) {
  auto walk = std::unordered_map<Point, size_t>{};
  auto next = std::vector<Point>{};
  next.push_back(entry);

  auto step = long{};
  while (step != steps && !next.empty()) {
    auto nextup = std::vector<Point>{};
    while (!next.empty()) {
      auto n = next.back();
      next.pop_back();
      if (walk.contains(n))
        continue;

      walk.emplace(n, step);
      for (auto &dir : std::vector<Point>{P::up, P::down, P::left, P::right}) {
        auto prospective = n + dir;

        auto in_range = prospective;

        if (!input.garden.contains(in_range))
          continue;
        if (walk.contains(prospective))
          continue;
        nextup.push_back(prospective);
      }
    }
    std::swap(next, nextup);
    ++step;
  }
  auto rval = size_t{};
  for (auto &w : walk)
    if (w.second % 2 == 0)
      ++rval;
  return rval;
}

constexpr auto long_walk = 26501365;

auto part2(const Parse &input) {
  BOOST_LOG_TRIVIAL(debug) << "long " << long_walk / input.width;
  auto wide = long_walk / input.width;
  BOOST_LOG_TRIVIAL(debug) << "long " << long_walk % input.width;

  auto evens = count_in_square(input, input.start, -1);
  auto odds = count_in_square(input, {64, 65}, -1);

  auto l = count_in_square(input, {0, 65}, 131);
  auto bl = count_in_square(input, {0, 130}, 66);

  auto r = count_in_square(input, {130, 65}, 131);
  auto br = count_in_square(input, {130, 130}, 66);

  auto d = count_in_square(input, {65, 130}, 131);
  auto u = count_in_square(input, {65, 0}, 131);

  auto ul = count_in_square(input, {0, 0}, 66);
  auto ur = count_in_square(input, {130, 0}, 66);

  BOOST_LOG_TRIVIAL(debug) << "ev: " << evens << " odds: " << odds << " l: " << l << " bl: " << bl
                           << " d: " << d << " br: " << br << " r: " << r << " ur: " << ur
                           << " u: " << u << " ul: " << ul;

  auto full_count = evens;
  auto step = 4;
  for (auto w = 1L; w < wide; ++w) {
    full_count += step * (w % 2 == 0 ? evens : odds);
    step += 4;
  }
  BOOST_LOG_TRIVIAL(debug) << "fc: " << full_count;

  return full_count + (bl + br + ul + ur) * (wide - 1) + u + d + l + r;
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/21.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input, 6);
  BOOST_LOG_TRIVIAL(info) << "Part 2: "
                          << part2(input); // > 1860427874359, !614865380257835, !614859278076818
}
