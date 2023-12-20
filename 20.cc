#include "parsing.h"
#include <algorithm>
#include <boost/log/trivial.hpp>
#include <fstream>
#include <list>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

struct SendList {
  bool pulse;
  std::vector<std::string> *targets;
};

struct Module {
  std::unordered_map<std::string, bool> inputs{};
  std::vector<std::string> outputs{};

  virtual ~Module() = default;
  virtual auto receive(bool pulse, const std::string &source) -> SendList = 0;
};

struct FlipFlop : public Module {
  bool state;

  auto receive(bool pulse, const std::string &) -> SendList override {
    static auto empty = std::vector<std::string>{};
    if (pulse)
      return {false, &empty};
    state = !state;
    return {state, &outputs};
  }
};

struct Conjunction : public Module {
  auto receive(bool pulse, const std::string &source) -> SendList override {
    inputs[source] = pulse;
    auto memory = std::all_of(inputs.begin(), inputs.end(), [](auto &i) {
      return i.second;
    });
    return {!memory, &outputs};
  }
};

struct Broadcast : public Module {
  auto receive(bool pulse, const std::string &) -> SendList override {
    return {pulse, &outputs};
  }
};

struct Output : public Module {
  auto receive(bool, const std::string &) -> SendList override {
    static auto empty = std::vector<std::string>{};
    return {false, &empty};
  }
};

using Modules = std::unordered_map<std::string, std::unique_ptr<Module>>;

auto parse(const std::string &filename) {
  auto rval = Modules{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  while (std::getline(input_handle, line)) {
    auto ss = std::stringstream{line};
    auto name = std::string{};
    ss >> name >> line;
    auto dest = std::vector<std::string>{};
    while (ss.good()) {
      ss >> line;
      if (line.back() == ',')
        dest.push_back(line.substr(0, line.size() - 1));
      else
        dest.push_back(line);
    }
    if (name == "broadcaster") {
      auto b = std::make_unique<Broadcast>();
      std::swap(b->outputs, dest);
      rval.insert({name, std::move(b)});
    } else if (name.at(0) == '%') {
      auto b = std::make_unique<FlipFlop>();
      std::swap(b->outputs, dest);
      rval.insert({name.substr(1), std::move(b)});
    } else if (name.at(0) == '&') {
      auto b = std::make_unique<Conjunction>();
      std::swap(b->outputs, dest);
      rval.insert({name.substr(1), std::move(b)});
    } else {
      throw std::runtime_error{"?"};
    }
  }
  for (auto &i : rval)
    for (auto &o : i.second->outputs) {
      if (!rval.contains(o))
        rval.insert({o, std::make_unique<Output>()});
      rval.at(o)->inputs.insert({i.first, false});
    }
  return rval;
}

struct Pulse {
  std::string source;
  bool pulse;
  std::string target;
};

auto part1(const Modules &modules) {
  auto low = size_t{};
  auto high = size_t{};
  for (auto pushes = 0; pushes < 1000; ++pushes) {
    auto pulses = std::list<Pulse>{};
    pulses.push_back({"button", false, "broadcaster"});
    while (!pulses.empty()) {
      auto next = pulses.front();
      pulses.pop_front();

      if (next.pulse)
        ++high;
      else
        ++low;

      auto send_list = modules.at(next.target)->receive(next.pulse, next.source);
      for (auto &s : *send_list.targets)
        pulses.push_back({next.target, send_list.pulse, s});
    }
  }
  return low * high;
}

auto part2(const Modules &modules) {
  auto pushes = size_t{};
  auto feeders = std::unordered_map<std::string, size_t>{};
  auto feedname = modules.at("rx")->inputs.begin()->first;
  for (auto &f : modules.at(feedname)->inputs) {
    BOOST_LOG_TRIVIAL(debug) << "  feeder: " << f.first;
    feeders.emplace(f.first, size_t{});
  }
  while (true) {
    ++pushes;
    auto pulses = std::list<Pulse>{};
    pulses.push_back({"button", false, "broadcaster"});
    while (!pulses.empty()) {
      auto next = pulses.front();
      pulses.pop_front();

      if (next.pulse)
        if (auto find = feeders.find(next.source); find != feeders.end() && find->second == 0)
          find->second = pushes;

      if (auto product = std::accumulate(feeders.begin(), feeders.end(), size_t{1},
                                         [](auto a, auto &b) {
                                           return a * b.second;
                                         });
          product != 0)
        return product;

      auto send_list = modules.at(next.target)->receive(next.pulse, next.source);
      for (auto &s : *send_list.targets)
        pulses.push_back({next.target, send_list.pulse, s});
    }
  }
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/20.txt");
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input); // 670,984,704
  input = parse("input/20.txt");
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << part2(input); // 262,775,362,119,547
}
