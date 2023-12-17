#pragma once

#include "point.h"
#include <istream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

constexpr auto char_to_num(char c) {
  return c - '0';
}

template <class T>
struct NumericMap {
  std::unordered_map<Point, T> map{};
  int width;
  int height;
};

template <typename F>
auto parse_map(std::istream &input_handle, F conv_function) {
  auto rval = NumericMap<decltype(conv_function('0'))>{};
  auto y = 0;
  auto line = std::string{};
  while (std::getline(input_handle, line)) {
    rval.width = std::max(rval.width, int(line.size()));
    for (auto x = 0; x < rval.width; ++x)
      rval.map.insert({Point{x, y}, conv_function(line.at(x))});
    ++y;
  }
  rval.height = y;
  return rval;
}

inline auto parse_strings(std::istream &input_handle) {
  auto rval = std::vector<std::string>{};
  auto line = std::string{};
  while (std::getline(input_handle, line))
    rval.push_back(line);
  return rval;
}

inline auto getline(std::istream &input_handle, std::stringstream &stream) {
  static auto str = std::string{};
  auto &read = std::getline(input_handle, str);
  if (read) {
    auto ss = std::stringstream{str};
    std::swap(ss, stream);
    return true;
  } else {
    return false;
  }
}
