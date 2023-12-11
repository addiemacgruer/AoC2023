#include "point.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Galaxy {
  std::vector<Point> gals{};
  int width{};
  int height{};
};

auto parse(const std::string &filename) {
  auto rval = Galaxy{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  auto y = int{};
  while (std::getline(input_handle, line)) {
    rval.width = std::max(rval.width, int(line.size()));
    for (auto x = int{}; x < int(line.size()); ++x)
      if (line.at(x) == '#')
        rval.gals.push_back({x, y});
    ++y;
  }
  rval.height = y;
  return rval;
}

auto expand(const Galaxy &in, int expansion) {
  auto square = std::max(in.height, in.width);
  auto col_count = std::vector<int>(square);
  auto row_count = std::vector<int>(square);
  for (auto &galaxy : in.gals) {
    col_count[galaxy.x]++;
    row_count[galaxy.y]++;
  }
  auto col_inc = std::vector<int>(square);
  auto row_inc = std::vector<int>(square);
  auto col_exp = int{};
  auto row_exp = int{};
  for (auto i = int{}; i < square; ++i) {
    if (col_count[i] == 0)
      col_exp += expansion - 1;
    col_inc[i] = col_exp;
    if (row_count[i] == 0)
      row_exp += expansion - 1;
    row_inc[i] = row_exp;
  }
  auto out = Galaxy{};
  std::transform(in.gals.begin(), in.gals.end(), std::back_inserter(out.gals),
                 [&](const Point &g) {
                   return Point{g.x + col_inc[g.x], g.y + row_inc[g.y]};
                 });
  return out;
}

auto measure_distances(const Galaxy &unexpanded, int expansion = 2) {
  auto expanded = expand(unexpanded, expansion);
  auto rval = size_t{};
  for (auto first = size_t{}; first < expanded.gals.size() - 1; ++first)
    for (auto second = size_t{first} + 1; second < expanded.gals.size(); ++second)
      rval += manhattan(expanded.gals.at(first), expanded.gals.at(second));
  return rval;
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/11.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << measure_distances(input);          // 9522407
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << measure_distances(input, 1000000); // 544723432977
}
