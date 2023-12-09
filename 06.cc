#include <boost/log/trivial.hpp>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

using Size = uint64_t;
using Parse = std::vector<std::pair<Size, Size>>;

auto test = Parse{{7, 9}, {15, 40}, {30, 200}};
auto test2 = Parse{{71530, 940200}};

auto puzz = Parse{{47, 282}, {70, 1079}, {75, 1147}, {66, 1062}};
auto puzz2 = Parse{{47707566, 282107911471062}};

auto find_distance_combos(const Parse &input) {
  auto rval = Size{1};
  for (auto &p : input) {
    auto firstwin = Size{};
    auto lastwin = Size{};
    for (auto charge = Size{1}; charge < p.first; ++charge) {
      auto distance = (p.first - charge) * charge;
      if (distance > p.second) {
        if (!firstwin)
          firstwin = charge;
        lastwin = charge;
      } else {
        if (firstwin)
          break;
      }
    }
    rval *= (lastwin - firstwin + 1);
  }
  return rval;
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << find_distance_combos(puzz);  // 281600
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << find_distance_combos(puzz2); // 33875953
}
