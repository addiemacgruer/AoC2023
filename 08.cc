#include <algorithm>
#include <boost/log/trivial.hpp>
#include <fstream>
#include <functional>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

struct Step {
  std::string left;
  std::string right;
};

struct Desert {
  std::string path;
  std::unordered_map<std::string, Step> steps;
};

auto parse(const std::string &filename) {
  auto rval = Desert{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  auto s = std::string{};
  std::getline(input_handle, rval.path);
  std::getline(input_handle, s);
  while (std::getline(input_handle, line)) {
    auto src = line.substr(0, 3);
    auto left = line.substr(7, 3);
    auto right = line.substr(12, 3);
    rval.steps.insert({src, {left, right}});
  }
  return rval;
}

auto walk(const Desert &input, std::string start, std::function<bool(std::string)> dest) {
  auto step = 0;
  auto place = std::move(start);
  while (!dest(place)) {
    auto next = input.path[step % input.path.size()];
    if (next == 'L')
      place = input.steps.at(place).left;
    else
      place = input.steps.at(place).right;
    ++step;
  }
  return step;
}

auto part1(const Desert &input) {
  return walk(input, "AAA", [](const std::string &place) {
    return place == "ZZZ";
  });
}

auto part2(const Desert &input) {
  auto loops = std::vector<size_t>{};
  for (auto &step : input.steps)
    if (step.first.at(2) == 'A')
      loops.push_back(walk(input, step.first, [](const std::string &place) {
        return place.at(2) == 'Z';
      }));
  return std::reduce(loops.begin(), loops.end(), size_t{1}, [](size_t a, size_t b) {
    return std::lcm(a, b);
  });
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/08.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input); // 19099
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << part2(input); // 17099847107071
}
