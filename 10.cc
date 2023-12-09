#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

using Parse = std::vector<std::string>;

auto parse(const std::string &filename) {
  auto rval = Parse{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  while (std::getline(input_handle, line)) {
    auto ss = std::stringstream{line};
    rval.push_back(line);
  }
  return rval;
}

auto part1(const Parse &input) {
  auto rval = 0;
  return rval;
}

auto part2(const Parse &input) {
  auto rval = 0;
  return rval;
}

} // namespace

auto main() -> int {
  auto input = parse("input/10.tst");
  std::cout << "Part 1: " << part1(input) << '\n';
  std::cout << "Part 2: " << part2(input) << '\n';
}
