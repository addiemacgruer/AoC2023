#include "parsing.h"
#include <bitset>
#include <boost/log/trivial.hpp>
#include <fstream>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

auto dirs = std::vector<Point>{P::right, P::down, P::left, P::up};

using Map = std::unordered_map<Point, int>;

using Factory = NumericMap<int>;

auto parse(const std::string &filename) {
  auto rval = Factory{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  return parse_map(input_handle, char_to_num);
}

struct Node {
  int heat;
  Point pos;
  int dir;
};

auto operator<[[maybe_unused]] (const Node &a, const Node &b) {
  return a.heat >= b.heat;
}

auto dijkstra(const Factory &factory, int mini, int maxi) {
  auto unvisited = std::priority_queue<Node>{};
  auto visited = std::unordered_map<Point, std::bitset<4>>{};
  unvisited.emplace(0, Point{0, 0}, -1);
  while (!unvisited.empty()) {
    auto node = unvisited.top();
    unvisited.pop();

    if (node.pos.x == factory.width - 1 && node.pos.y == factory.height - 1)
      return node.heat;
    if (node.dir != -1) {
      if (visited[node.pos].test(node.dir))
        continue;
      visited[node.pos].set(node.dir);
    }

    for (auto d = 0; d < int(dirs.size()); ++d) {
      if (d != -1 && (d == node.dir || (d + 2) % 4 == node.dir))
        continue;
      auto prospective = node.pos;
      auto p_heat = node.heat;
      for (auto step = 1; step <= maxi; ++step) {
        prospective = prospective + dirs.at(d);
        if (prospective.x < 0 || prospective.y < 0 || prospective.x >= factory.width ||
            prospective.y >= factory.height)
          break;
        p_heat += factory.map.at(prospective);
        if (step >= mini)
          unvisited.emplace(p_heat, prospective, d);
      }
    }
  }
  throw std::runtime_error{"lost"};
}

auto part1(const Factory &factory) {
  return dijkstra(factory, 1, 3);
}

auto part2(const Factory &factory) {
  return dijkstra(factory, 4, 10);
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/17.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input); // 694
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << part2(input); // 829
}
