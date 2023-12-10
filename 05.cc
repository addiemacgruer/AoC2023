#include <boost/log/trivial.hpp>
#include <fstream>
#include <future>
#include <limits>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Mapping {
  size_t dest;
  size_t source;
  size_t range;
};

struct Input {
  std::vector<size_t> seeds{};
  std::vector<std::vector<Mapping>> mappings{};
};

auto parse_mappings(std::vector<Mapping> &map, std::istream &input) {
  auto line = std::string{};
  std::getline(input, line);
  auto dest = size_t{};
  auto source = size_t{};
  auto range = size_t{};
  while (true) {
    std::getline(input, line);
    if (line.empty())
      return;
    auto ss = std::stringstream{line};
    ss >> dest >> source >> range;
    map.push_back({dest, source, range});
  }
}

auto parse(const std::string &filename) {
  auto rval = Input{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  auto s = std::string{};
  auto v = size_t{};
  std::getline(input_handle, line);
  auto ss = std::stringstream{line};
  ss >> s;
  while (ss >> v && v)
    rval.seeds.push_back(v);
  std::getline(input_handle, line);
  while (input_handle) {
    rval.mappings.emplace_back();
    parse_mappings(rval.mappings.back(), input_handle);
  }
  return rval;
}

auto find_next(size_t input, const std::vector<Mapping> &maps) {
  for (const auto &map : maps)
    if (input >= map.source && input < map.source + map.range)
      return map.dest + input - map.source;
  return input;
}

auto seed2loc(size_t seed, const Input &input) {
  return std::accumulate(input.mappings.begin(), input.mappings.end(), seed,
                         [](size_t in, const std::vector<Mapping> &mapping) {
                           return find_next(in, mapping);
                         });
}

auto part1(const Input &input) {
  auto rval = std::numeric_limits<size_t>::max();
  for (auto seed : input.seeds)
    rval = std::min(seed2loc(seed, input), rval);
  return rval;
}

auto part2(const Input &input) {
  auto futures = std::vector<std::future<size_t>>{};
  for (auto i = size_t{}; i < input.seeds.size(); i += 2) {
    auto lower = input.seeds.at(i);
    auto upper = lower + input.seeds.at(i + 1);
    futures.push_back(std::async([&input, lower, upper]() {
      auto loc_rval = std::numeric_limits<size_t>::max();
      for (auto seed = lower; seed < upper; ++seed)
        loc_rval = std::min(seed2loc(seed, input), loc_rval);
      return loc_rval;
    }));
  }
  auto rval = std::numeric_limits<size_t>::max();
  for (auto &future : futures)
    rval = std::min(rval, future.get());
  return rval;
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/05.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input); // 662197086
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << part2(input); // 52510809
}
