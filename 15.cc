#include <boost/log/trivial.hpp>
#include <fstream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using Parse = std::vector<std::string>;

auto parse(const std::string &filename) {
  auto rval = Parse{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  std::getline(input_handle, line);
  auto ss = std::stringstream{line};
  auto s = std::string{};
  while (ss) {
    std::getline(ss, s, ',');
    rval.push_back(s);
  }
  return rval;
}

auto hash(const std::string &word) {
  auto rval = int{};
  for (auto c : word) {
    rval += c;
    rval *= 17;
    rval %= 256;
  }
  return rval;
}

auto part1(const Parse &input) {
  return std::accumulate(input.begin(), input.end(), 0L, [](auto count, auto &word) {
    return count + hash(word);
  });
}

struct Lens {
  std::string code;
  int power;
};

using Boxes = std::vector<std::vector<Lens>>;

auto dash(Boxes &boxes, const std::string &lens) {
  auto box = hash(lens);
  for (auto slot = size_t{}; slot < boxes.at(box).size(); ++slot)
    if (boxes.at(box).at(slot).code == lens) {
      boxes.at(box).erase(boxes.at(box).begin() + slot);
      return;
    }
}

auto equals(Boxes &boxes, const Lens &lens) {
  auto box = hash(lens.code);
  for (auto slot = size_t{}; slot < boxes.at(box).size(); ++slot)
    if (boxes.at(box).at(slot).code == lens.code) {
      boxes.at(box).at(slot).power = lens.power;
      return;
    }
  boxes.at(box).push_back(lens);
}

auto part2(const Parse &input) {
  auto boxes = Boxes(256);
  for (auto &lens_code : input) {
    if (lens_code.at(lens_code.size() - 1) == '-') {
      dash(boxes, lens_code.substr(0, lens_code.size() - 1));
    } else {
      auto eq = lens_code.find('=');
      equals(boxes, {lens_code.substr(0, eq), std::stoi(lens_code.substr(eq + 1))});
    }
  }
  auto rval = 0L;
  for (auto box = size_t{}; box < boxes.size(); ++box)
    for (auto slot = size_t{}; slot < boxes.at(box).size(); ++slot)
      rval += (box + 1) * (slot + 1) * boxes.at(box).at(slot).power;
  return rval;
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/15.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input); // 495972
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << part2(input); // 245223
}
