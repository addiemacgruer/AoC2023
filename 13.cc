#include <boost/log/trivial.hpp>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using Mirror = std::vector<std::string>;
using Parse = std::vector<Mirror>;

auto parse(const std::string &filename) {
  auto rval = Parse{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  while (input_handle) {
    rval.emplace_back();
    while (std::getline(input_handle, line)) {
      if (line.empty())
        break;
      rval.back().push_back(line);
    }
  }
  return rval;
}

template <int smudges>
auto hori_reflect(const Mirror &mirror) {
  auto width = int(mirror.at(0).size());
  auto height = int(mirror.size());
  for (auto i = 0; i < width - 1; ++i) {
    auto smudge = 0;
    for (auto y = 0; y < height; ++y) {
      for (auto x = i; x < width; ++x) {
        auto offs = x - i;
        if (i - offs < 0 || 1 + i + offs >= width)
          continue;
        if (mirror.at(y).at(1 + i + offs) != mirror.at(y).at(i - offs))
          ++smudge;
        if (smudge > smudges)
          goto next;
      }
    }
    if (smudge == smudges)
      return i + 1;
  next:
    continue;
  }
  return 0;
}

template <int smudges>
auto vert_reflect(const Mirror &mirror) {
  auto width = int(mirror.at(0).size());
  auto height = int(mirror.size());
  for (auto i = 0; i < height; ++i) {
    auto smudge = 0;
    for (auto x = 0; x < width; ++x) {
      for (auto y = i; y < height; ++y) {
        auto offs = y - i;
        if (i - offs < 0 || 1 + i + offs >= height)
          continue;
        if (mirror.at(1 + i + offs).at(x) != mirror.at(i - offs).at(x))
          ++smudge;
        if (smudge > smudges)
          goto next;
      }
    }
    if (smudge == smudges)
      return 100 * (i + 1);
  next:
    continue;
  }
  return 0;
}

template <int smudges>
auto count_reflections(const Parse &input) {
  auto rval = size_t{};
  for (auto &mirror : input) {
    if (auto h = hori_reflect<smudges>(mirror))
      rval += h;
    else if (auto v = vert_reflect<smudges>(mirror))
      rval += v;
    else
      throw std::runtime_error{"vampire"};
  }
  return rval;
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/13.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << count_reflections<0>(input); // 30535
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << count_reflections<1>(input); // 30844
}
