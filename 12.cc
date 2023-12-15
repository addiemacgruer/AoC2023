#include "point.h"
#include <algorithm>
#include <boost/log/trivial.hpp>
#include <fstream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

struct Spring {
  std::string pattern;
  std::vector<int> lengths{};
};

using Springs = std::vector<Spring>;

auto parse(const std::string &filename) {
  auto rval = Springs{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  while (std::getline(input_handle, line)) {
    rval.emplace_back();
    auto numbers = std::string{};
    auto ss = std::stringstream{line};
    ss >> rval.back().pattern >> numbers;
    auto sss = std::stringstream(numbers);
    auto num = std::string{};
    while (sss.good()) {
      std::getline(sss, num, ',');
      rval.back().lengths.push_back(std::stoi(num));
    }
  }
  return rval;
}

constexpr auto could_be_dot(char c) {
  return c == '.' || c == '?';
}

constexpr auto could_be_hash(char c) {
  return c == '#' || c == '?';
}

auto cache = std::unordered_map<Point, size_t>{};

auto count_possibilities(const Spring &spring, const std::vector<int> &endpoints, int start,
                         size_t depth) -> size_t {
  auto point = Point{start, int(depth)};
  if (auto find = cache.find(point); find != cache.end())
    return find->second;
  auto total = size_t{};
  for (auto i = start; i <= endpoints.at(depth); ++i) {
    auto trial = start;
    while (trial < i) {
      if (!could_be_dot(spring.pattern.at(trial)))
        goto bad;
      ++trial;
    }
    for (auto c = int{}; c < spring.lengths.at(depth); ++c) {
      if (!could_be_hash(spring.pattern.at(trial)))
        goto bad;
      ++trial;
    }

    if (trial < int(spring.pattern.size())) {
      if (!could_be_dot(spring.pattern.at(trial)))
        goto bad;
      ++trial;
    }

    if (depth + 1 == endpoints.size()) {
      while (trial < int(spring.pattern.size())) {
        if (!could_be_dot(spring.pattern.at(trial)))
          goto bad;
        ++trial;
      }
      ++total;
    } else {
      total += count_possibilities(spring, endpoints, trial, depth + 1);
    }
  bad:
    continue;
  }
  cache.insert({point, total});
  return total;
}

auto count_possibilities(const Spring &s) {
  auto end = std::vector<int>(s.lengths.size());
  auto count = int(s.pattern.size());
  for (auto i = int(s.lengths.size()) - 1; i >= 0; --i) {
    count -= s.lengths.at(i);
    end.at(i) = count;
    --count;
  }
  cache.clear();
  return count_possibilities(s, end, 0, 0);
}

auto part1(const Springs &input) {
  return std::accumulate(input.cbegin(), input.cend(), size_t{}, [](auto a, const auto &b) {
    return a + count_possibilities(b);
  });
}

auto unfold(const Spring &s) {
  auto rval = Spring{};
  for (auto i = 0; i < 5; ++i) {
    if (!rval.pattern.empty())
      rval.pattern.push_back('?');
    rval.pattern.append(s.pattern);
    std::copy(s.lengths.begin(), s.lengths.end(), std::back_inserter(rval.lengths));
  }
  return rval;
}

auto part2(const Springs &input) {
  return std::accumulate(input.cbegin(), input.cend(), size_t{}, [](auto a, const auto &b) {
    return a + count_possibilities(unfold(b));
  });
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/12.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input); // 7792
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << part2(input); // 13012052341533
}
