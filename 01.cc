#include "parsing.h"
#include <boost/log/trivial.hpp>
#include <fstream>
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
  auto ih = std::ifstream{filename};
  if (!ih)
    throw std::runtime_error{"could not open file"};
  return parse_strings(ih);
}

struct is_digit {
  auto operator()(const std::string &line, int i) {
    auto c = line[i];
    return (c >= '1' && c <= '9') ? c - '0' : -1;
  }
};

struct is_digit_or_name {
  auto operator()(const std::string &line, int i) {
    if (auto v = is_digit{}(line, i); v != -1)
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
};

template <typename Evaluator>
auto part(const std::vector<std::string> &input) {
  auto evaluator = Evaluator{};
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
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/01.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part1: " << part<is_digit>(input);         // 55816
  BOOST_LOG_TRIVIAL(info) << "Part2: " << part<is_digit_or_name>(input); // 54980
}
