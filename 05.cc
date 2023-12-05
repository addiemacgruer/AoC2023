#include <fstream>
#include <future>
#include <iostream>
#include <limits>
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
  std::vector<Mapping> seed2soil{};
  std::vector<Mapping> soil2fert{};
  std::vector<Mapping> fert2water{};
  std::vector<Mapping> water2light{};
  std::vector<Mapping> light2temp{};
  std::vector<Mapping> temp2humi{};
  std::vector<Mapping> humi2loc{};
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
  parse_mappings(rval.seed2soil, input_handle);
  parse_mappings(rval.soil2fert, input_handle);
  parse_mappings(rval.fert2water, input_handle);
  parse_mappings(rval.water2light, input_handle);
  parse_mappings(rval.light2temp, input_handle);
  parse_mappings(rval.temp2humi, input_handle);
  parse_mappings(rval.humi2loc, input_handle);

  return rval;
}

auto find_next(size_t input, const std::vector<Mapping> &maps) {
  for (const auto &map : maps)
    if (input >= map.source && input < map.source + map.range)
      return map.dest + input - map.source;
  return input;
}

auto seed2loc(size_t seed, const Input &input) {
  auto soil = find_next(seed, input.seed2soil);
  auto fert = find_next(soil, input.soil2fert);
  auto water = find_next(fert, input.fert2water);
  auto light = find_next(water, input.water2light);
  auto temp = find_next(light, input.light2temp);
  auto humi = find_next(temp, input.temp2humi);
  return find_next(humi, input.humi2loc);
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
  auto input = parse("05.txt");
  std::cout << "Part 1: " << part1(input) << '\n'; // 662197086
  std::cout << "Part 2: " << part2(input) << '\n'; // 52510809
}
