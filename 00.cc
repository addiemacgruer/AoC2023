#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using Parse = std::vector<std::string>;

auto parse(const std::string &filename) {
  auto rval = Parse{};
  auto ih = std::ifstream{filename};
  if (!ih)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  while (std::getline(ih, line)) {
    rval.push_back(line);
  }
  return rval;
}

} // namespace

auto main() -> int {
  auto input = parse("00.tst");
}
