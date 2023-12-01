#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

const auto names = std::vector<std::pair<std::string, int>>{
    {"one", 1}, {"two", 2},   {"three", 3}, {"four", 4}, {"five", 5},
    {"six", 6}, {"seven", 7}, {"eight", 8}, {"nine", 9}};

auto parse(const std::string &filename) {
  auto rval = std::vector<std::string>{};
  auto ih = std::ifstream{filename};
  if (!ih)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  while (std::getline(ih, line))
    rval.push_back(line);
  return rval;
}

auto is_digit(const std::string &line, int i) {
  auto c = line[i];
  return (c >= '1' && c <= '9') ? c - '0' : -1;
}

auto is_digit_or_name(const std::string &line, int i) {
  if (auto v = is_digit(line, i); v != -1)
    return v;
  for (const auto &e : names) {
    if (i + e.first.size() > line.size() || e.first[0] != line[i]) [[likely]]
      continue;
    auto ss = std::string_view{line}.substr(i, e.first.size());
    if (ss == e.first) [[unlikely]]
      return e.second;
  }
  return -1;
}

template <class F>
auto part(const std::vector<std::string> &input, F evaluator) {
  auto rval = 0;
  for (const auto &line : input) {
    auto digit = 0;
    for (auto i = 0; i < int(line.size()); ++i) {
      if (auto v = evaluator(line, i); v != -1) {
        digit += v * 10;
        break;
      }
    }
    for (auto i = int(line.size()) - 1; i >= 0; --i) {
      if (auto v = evaluator(line, i); v != -1) {
        digit += v;
        break;
      }
    }
    rval += digit;
  }
  return rval;
}

} // namespace

auto main() -> int {
  auto input = parse("01.txt");
  std::cout << "Part1: " << part(input, is_digit) << std::endl;         // 55816
  std::cout << "Part2: " << part(input, is_digit_or_name) << std::endl; // 54980
}
