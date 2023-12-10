#include "point.h"
#include <algorithm>
#include <boost/log/trivial.hpp>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

auto left = Point{-1, 0};
auto right = Point{1, 0};
auto up = Point{0, -1};
auto down = Point{0, 1};

auto pipes = std::unordered_map<char, std::vector<Point>>{
    {'|', {up, down}}, {'-', {left, right}}, {'L', {up, right}},
    {'J', {left, up}}, {'7', {left, down}},  {'F', {right, down}},
};

struct Maze {
  Point start;
  std::unordered_map<Point, char> pieces{};
  int xmax;
  int ymax;
};

auto goes(const std::vector<Point> &points, const Point &direction) {
  return points.at(0) == direction || points.at(1) == direction;
}

auto find_start(const Maze &input) {
  auto start = std::vector<Point>{};
  for (auto &adjacent : std::vector{up, down, left, right}) {
    auto test = input.start + adjacent;
    if (!input.pieces.contains(test))
      continue;
    auto backwards = Point{} - adjacent;
    if (goes(pipes.at(input.pieces.at(test)), backwards))
      start.push_back(adjacent);
  }
  if (start.size() != 2)
    throw std::runtime_error{"bad start"};
  for (auto &pipe : pipes)
    if (std::is_permutation(pipe.second.begin(), pipe.second.end(), start.begin(), start.end()))
      return pipe.first;
  throw std::runtime_error{"couldn't decide where we started"};
}

auto parse(const std::string &filename) {
  auto rval = Maze{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  auto y = 0;
  while (std::getline(input_handle, line)) {
    rval.xmax = std::max(rval.xmax, int(line.size()));
    for (auto x = 0; x < rval.xmax; ++x) {
      auto ps = line.at(x);
      if (ps == '.')
        continue;
      if (ps == 'S')
        rval.start = Point{x, y};
      else
        rval.pieces.insert({Point{x, y}, ps});
    }
    ++y;
  }
  rval.ymax = y;
  rval.pieces.insert({rval.start, find_start(rval)});
  return rval;
}

auto find_next(const Maze &input, const Point &last, const std::unordered_set<Point> &been) {
  auto ps = input.pieces.at(last);
  for (auto &n : pipes.at(ps)) {
    auto next = last + n;
    if (!been.contains(next) && input.pieces.contains(next))
      return next;
  }
  return input.start;
}

auto count_inside(const Maze &input, const std::unordered_set<Point> &loop) {
  auto inside = size_t{};
  for (auto y = 0; y <= input.ymax; ++y) {
    auto last_turn = char{};
    auto crossings = 0;
    for (auto x = 0; x <= input.xmax; ++x) {
      auto p = Point{x, y};
      if (loop.contains(p)) {
        auto next_turn = input.pieces.at(p);
        if (next_turn == '-')
          continue;
        if (next_turn == '|')
          ++crossings;
        if (last_turn == 'F' && next_turn == 'J')
          ++crossings;
        if (last_turn == 'L' && next_turn == '7')
          ++crossings;
        last_turn = next_turn;
      } else if (crossings % 2) {
        ++inside;
      }
    }
    if (crossings % 2)
      throw std::runtime_error{"uneven number of crossings"};
  }
  return inside;
}

auto draw_loop(const Maze &input) {
  auto loop = std::unordered_set<Point>{};
  loop.insert(input.start);
  auto dir = pipes.at(input.pieces.at(input.start)).at(0);
  auto next = input.start + dir;
  while (true) {
    loop.insert(next);
    next = find_next(input, next, loop);
    if (next == input.start)
      return loop;
  }
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/10.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  auto loop = draw_loop(input);
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << (loop.size() / 2);         // 6882
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << count_inside(input, loop); // 491
}
