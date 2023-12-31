#include "parsing.h"
#include "point.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <future>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

enum Dir { UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8 };

using Parse = std::vector<std::string>;

auto parse(const std::string &filename) {
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  return parse_strings(input_handle);
}

struct Beam {
  Point pos;
  Point dir;
};

auto energise(const Parse &input, const Beam &origin) {
  auto beams = std::vector<Beam>{};
  auto width = int(input.at(0).size());
  auto height = int(input.size());
  beams.push_back(origin);
  auto contraption = std::unordered_map<Point, int>{};
  while (!beams.empty()) {
    auto beam = beams.back();
    beams.pop_back();

    if (beam.dir == P::left && beam.pos.x < width) {
      if (contraption.contains(beam.pos) && contraption.at(beam.pos) & Dir::LEFT)
        continue;
      contraption[beam.pos] |= Dir::LEFT;
    }
    if (beam.dir == P::right && beam.pos.x >= 0) {
      if (contraption.contains(beam.pos) && contraption.at(beam.pos) & Dir::RIGHT)
        continue;
      contraption[beam.pos] |= Dir::RIGHT;
    }
    if (beam.dir == P::up && beam.pos.y < height) {
      if (contraption.contains(beam.pos) && contraption.at(beam.pos) & Dir::UP)
        continue;
      contraption[beam.pos] |= Dir::UP;
    }
    if (beam.dir == P::down && beam.pos.y >= 0) {
      if (contraption.contains(beam.pos) && contraption.at(beam.pos) & Dir::DOWN)
        continue;
      contraption[beam.pos] |= Dir::DOWN;
    }

    auto next = beam.dir + beam.pos;

    if (next.x < 0 || next.y < 0 || next.x >= width || next.y >= height)
      continue;

    if (input.at(next.y).at(next.x) == '.') {
      beams.push_back({next, beam.dir});
      continue;
    }

    if (input.at(next.y).at(next.x) == '/') {
      if (beam.dir == P::right)
        beams.push_back({next, P::up});
      else if (beam.dir == P::left)
        beams.push_back({next, P::down});
      else if (beam.dir == P::up)
        beams.push_back({next, P::right});
      else if (beam.dir == P::down)
        beams.push_back({next, P::left});
      continue;
    }

    if (input.at(next.y).at(next.x) == '\\') {
      if (beam.dir == P::right)
        beams.push_back({next, P::down});
      else if (beam.dir == P::left)
        beams.push_back({next, P::up});
      else if (beam.dir == P::up)
        beams.push_back({next, P::left});
      else if (beam.dir == P::down)
        beams.push_back({next, P::right});
      continue;
    }

    if (input.at(next.y).at(next.x) == '-') {
      if (beam.dir == P::right || beam.dir == P::left)
        beams.push_back({next, beam.dir});
      else {
        beams.push_back({next, P::left});
        beams.push_back({next, P::right});
      }
      continue;
    }

    if (input.at(next.y).at(next.x) == '|') {
      if (beam.dir == P::up || beam.dir == P::down)
        beams.push_back({next, beam.dir});
      else {
        beams.push_back({next, P::up});
        beams.push_back({next, P::down});
      }
      continue;
    }

    throw std::runtime_error{"lost track of beam?"};
  }
  return contraption.size();
}

auto part1(const Parse &input) {
  return energise(input, {{-1, 0}, P::right});
}

auto part2(const Parse &input) {
  auto futures = std::vector<std::future<size_t>>{};
  futures.push_back(std::async([&]() {
    auto rval = size_t{};
    for (auto x = 0; x < int(input.at(0).size()); ++x)
      rval = std::max(rval, energise(input, {{x, int(input.size())}, P::up}));
    return rval;
  }));
  futures.push_back(std::async([&]() {
    auto rval = size_t{};
    for (auto x = 0; x < int(input.at(0).size()); ++x)
      rval = std::max(rval, energise(input, {{x, -1}, P::down}));
    return rval;
  }));
  futures.push_back(std::async([&]() {
    auto rval = size_t{};
    for (auto y = 0; y < int(input.size()); ++y)
      rval = std::max(rval, energise(input, {{int(input.at(0).size())}, P::left}));
    return rval;
  }));
  futures.push_back(std::async([&]() {
    auto rval = size_t{};
    for (auto y = 0; y < int(input.size()); ++y)
      rval = std::max(rval, energise(input, {{-1, y}, P::right}));
    return rval;
  }));
  return std::accumulate(futures.begin(), futures.end(), size_t{}, [](auto a, auto &f) {
    return std::max(a, f.get());
  });
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/16.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input); // 8034
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << part2(input); // 8225
}
