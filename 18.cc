#include "parsing.h"
#include <algorithm>
#include <boost/log/trivial.hpp>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Dig {
  char direction;
  int distance;
  std::string colour;
};

using Parse = std::vector<Dig>;

auto parse(const std::string &filename) {
  auto rval = Parse{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto dir = std::string{};
  auto dist = int{};
  auto colour = std::string{};
  auto ss = std::stringstream{};
  while (getline(input_handle, ss)) {
    ss >> dir >> dist >> colour;
    rval.emplace_back(dir.at(0), dist, colour.substr(2, 6));
  }
  return rval;
}

auto shoelace(const std::vector<Point> &points) {
  auto area = size_t{};
  auto perimeter = size_t{};
  for (auto s = size_t{}; s < points.size(); ++s) {
    auto &a = points.at(s);
    auto &b = points.at(s == points.size() - 1 ? 0 : s + 1);
    area += long(a.x) * b.y - long(b.x) * a.y;
    perimeter += std::abs(a.x - b.x) + std::abs(a.y - b.y);
  }
  return area / 2 + perimeter / 2 + 1; // Pick's
}

struct Part1 {
  auto operator()(const Point current_position, const Dig &dig) {
    switch (dig.direction) {
    case 'R':
      return Point{current_position.x + dig.distance, current_position.y};
    case 'D':
      return Point{current_position.x, current_position.y + dig.distance};
    case 'L':
      return Point{current_position.x - dig.distance, current_position.y};
    case 'U':
      return Point{current_position.x, current_position.y - dig.distance};
    default:
      throw std::runtime_error{"bad direction"};
    }
  }
};

struct Part2 {
  auto operator()(const Point current_position, const Dig &dig) {
    auto dist = std::stoi(dig.colour.substr(0, 5), nullptr, 16);
    switch (dig.colour.at(5)) {
    case '0':
      return Point{current_position.x + dist, current_position.y};
    case '1':
      return Point{current_position.x, current_position.y + dist};
    case '2':
      return Point{current_position.x - dist, current_position.y};
    case '3':
      return Point{current_position.x, current_position.y - dist};
    default:
      throw std::runtime_error{"bad direction"};
    }
  }
};

template <class Digger>
auto measure_dig(const Parse &input) {
  auto current_position = Point{};
  auto dug = std::vector<Point>{};
  dug.reserve(input.size() + 1);
  dug.push_back(current_position);
  auto digger = Digger{};
  for (auto &dig : input)
    dug.push_back(digger(dug.back(), dig));
  return shoelace(dug);
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/18.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << measure_dig<Part1>(input); // 46334
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << measure_dig<Part2>(input); // 102000662718092
}
