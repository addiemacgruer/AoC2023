#include <algorithm>
#include <boost/log/trivial.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using Oasis = std::vector<std::vector<int>>;

auto parse(const std::string &filename) {
  auto rval = Oasis{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  while (std::getline(input_handle, line)) {
    auto ss = std::stringstream{line};
    rval.emplace_back();
    auto i = int{};
    while (ss >> i)
      rval.back().push_back(i);
  }
  return rval;
}

auto solve(const Oasis &sequences) {
  auto part1 = 0;
  auto part2 = 0;
  for (const auto &sequence : sequences) {
    auto ss = std::vector<std::vector<int>>{};
    ss.push_back(sequence);

    do {
      ss.emplace_back();
      auto &last = ss.at(ss.size() - 2);
      for (auto i = size_t{}; i < last.size() - 1; ++i)
        ss.back().push_back(last.at(i + 1) - last.at(i));
    } while (!std::all_of(ss.back().begin(), ss.back().end(), [](auto a) {
      return a == 0;
    }));

    for (auto i = int(ss.size()) - 2; i >= 0; --i) {
      ss.at(i).push_back(ss.at(i).back() + ss.at(i + 1).back());
      ss.at(i).insert(ss.at(i).begin(), ss.at(i).front() - ss.at(i + 1).front());
    }

    part1 += ss.front().back();
    part2 += ss.front().front();
  }
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1   // 1819125966
                          << " Part 2: " << part2; // 1140
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/09.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  solve(input);
}
