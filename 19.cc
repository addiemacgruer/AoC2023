#include "parsing.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

struct Rule {
  char cat;
  char op;
  int val;
  std::string wf;
};

using Rules = std::vector<Rule>;

struct Part {
  int x, m, a, s;

  auto score() const {
    return x + m + a + s;
  }
};

struct Parse {
  std::unordered_map<std::string, Rules> rules{};
  std::vector<Part> parts{};
};

auto parse(const std::string &filename) {
  auto rval = Parse{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  while (std::getline(input_handle, line)) {
    if (line.empty())
      break;
    auto open = line.find('{');
    auto rule_name = line.substr(0, open);
    auto ss = std::stringstream{line.substr(open + 1, line.size() - open - 2)};
    auto rules = Rules{};
    auto rule = std::string{};
    while (std::getline(ss, rule, ',')) {
      auto colon = rule.find(':');
      if (colon == std::string::npos) {
        rules.push_back({'\0', '=', 0, rule});
        continue;
      }
      rules.push_back({rule.at(0), rule.at(1), std::stoi(rule.substr(2, rule.size() - colon + 2)),
                       rule.substr(colon + 1)});
    }
    rval.rules[rule_name] = std::move(rules);
  }
  while (std::getline(input_handle, line)) {
    auto part = Part{};
    auto ss = std::stringstream{line.substr(1, line.size() - 2)};
    auto part_line = std::string{};
    while (std::getline(ss, part_line, ',')) {
      auto v = std::stoi(part_line.substr(2));
      switch (part_line.at(0)) {
      case 'x':
        part.x = v;
        break;
      case 'm':
        part.m = v;
        break;
      case 'a':
        part.a = v;
        break;
      case 's':
        part.s = v;
        break;
      default:
        throw std::runtime_error{"part?"};
      }
    }
    rval.parts.push_back(std::move(part));
  }
  return rval;
}

auto part1(const Parse &input) {
  auto rval = 0;
  for (auto &part : input.parts) {
    auto wf = std::string{"in"};
    while (true) {
      if (wf == "A") {
        rval += part.score();
        goto next_part;
      }
      if (wf == "R") {
        goto next_part;
      }
      for (auto &rule : input.rules.at(wf)) {
        if (rule.op == '=') {
          wf = rule.wf;
          goto next_workflow;
        } else if (rule.op == '<')
          switch (rule.cat) {
          case 'x':
            if (part.x < rule.val) {
              wf = rule.wf;
              goto next_workflow;
            }
            break;
          case 'm':
            if (part.m < rule.val) {
              wf = rule.wf;
              goto next_workflow;
            }
            break;
          case 'a':
            if (part.a < rule.val) {
              wf = rule.wf;
              goto next_workflow;
            }
            break;
          case 's':
            if (part.s < rule.val) {
              wf = rule.wf;
              goto next_workflow;
            }
            break;
          default:
            throw std::runtime_error{"<"};
          }
        else if (rule.op == '>')
          switch (rule.cat) {
          case 'x':
            if (part.x > rule.val) {
              wf = rule.wf;
              goto next_workflow;
            }
            break;
          case 'm':
            if (part.m > rule.val) {
              wf = rule.wf;
              goto next_workflow;
            }
            break;
          case 'a':
            if (part.a > rule.val) {
              wf = rule.wf;
              goto next_workflow;
            }
            break;
          case 's':
            if (part.s > rule.val) {
              wf = rule.wf;
              goto next_workflow;
            }
            break;
          default:
            throw std::runtime_error{"<"};
          }
        else
          throw std::runtime_error{"?"};
      }
    next_workflow:
      continue;
    }
  next_part:
    continue;
  }
  return rval;
}

struct Range {
  long min, max;

  auto size() const {
    return max - min + 1;
  }
};

struct PartRange {
  Range x, m, a, s;

  auto score() const {
    return x.size() * m.size() * a.size() * s.size();
  }
};

struct Stage {
  PartRange pr;
  std::string wf;
  size_t step;
};

auto part2([[maybe_unused]] const Parse &input) {
  auto rval = size_t{};
  auto q = std::stack<Stage>{};
  q.emplace(PartRange{Range{1, 4000}, Range{1, 4000}, Range{1, 4000}, Range{1, 4000}}, "in",
            size_t{});
  while (!q.empty()) {
    auto p = q.top();
    q.pop();

    auto &wf = p.wf;
    if (wf == "A") {
      rval += p.pr.score();
      continue;
    }
    if (wf == "R") {
      continue;
    }
    auto rcount = p.step;
    auto &rule = input.rules.at(wf).at(p.step);
    if (rule.op == '=') {
      q.emplace(p.pr, rule.wf, 0);
    } else if (rule.op == '<')
      switch (rule.cat) {
      case 'x':
        q.emplace(PartRange{{p.pr.x.min, rule.val - 1}, p.pr.m, p.pr.a, p.pr.s}, rule.wf, 0);
        q.emplace(PartRange{{rule.val, p.pr.x.max}, p.pr.m, p.pr.a, p.pr.s}, wf, rcount + 1);
        break;
      case 'm':
        q.emplace(PartRange{p.pr.x, {p.pr.m.min, rule.val - 1}, p.pr.a, p.pr.s}, rule.wf, 0);
        q.emplace(PartRange{p.pr.x, {rule.val, p.pr.m.max}, p.pr.a, p.pr.s}, wf, rcount + 1);
        break;
      case 'a':
        q.emplace(PartRange{p.pr.x, p.pr.m, {p.pr.a.min, rule.val - 1}, p.pr.s}, rule.wf, 0);
        q.emplace(PartRange{p.pr.x, p.pr.m, {rule.val, p.pr.a.max}, p.pr.s}, wf, rcount + 1);
        break;
      case 's':
        q.emplace(PartRange{p.pr.x, p.pr.m, p.pr.a, {p.pr.s.min, rule.val - 1}}, rule.wf, 0);
        q.emplace(PartRange{p.pr.x, p.pr.m, p.pr.a, {rule.val, p.pr.s.max}}, wf, rcount + 1);
        break;
      default:
        throw std::runtime_error{"<"};
      }
    else if (rule.op == '>')
      switch (rule.cat) {
      case 'x':
        q.emplace(PartRange{{p.pr.x.min, rule.val}, p.pr.m, p.pr.a, p.pr.s}, wf, rcount + 1);
        q.emplace(PartRange{{rule.val + 1, p.pr.x.max}, p.pr.m, p.pr.a, p.pr.s}, rule.wf, 0);
        break;
      case 'm':
        q.emplace(PartRange{p.pr.x, {p.pr.m.min, rule.val}, p.pr.a, p.pr.s}, wf, rcount + 1);
        q.emplace(PartRange{p.pr.x, {rule.val + 1, p.pr.m.max}, p.pr.a, p.pr.s}, rule.wf, 0);
        break;
      case 'a':
        q.emplace(PartRange{p.pr.x, p.pr.m, {p.pr.a.min, rule.val}, p.pr.s}, wf, rcount + 1);
        q.emplace(PartRange{p.pr.x, p.pr.m, {rule.val + 1, p.pr.a.max}, p.pr.s}, rule.wf, 0);
        break;
      case 's':
        q.emplace(PartRange{p.pr.x, p.pr.m, p.pr.a, {p.pr.s.min, rule.val}}, wf, rcount + 1);
        q.emplace(PartRange{p.pr.x, p.pr.m, p.pr.a, {rule.val + 1, p.pr.s.max}}, rule.wf, 0);
        break;
      default:
        throw std::runtime_error{">"};
      }
    else
      throw std::runtime_error{"?"};
  }
  return rval;
}

} // namespace

auto main() -> int {
  BOOST_LOG_TRIVIAL(debug) << "Starting up";
  auto input = parse("input/19.txt");
  BOOST_LOG_TRIVIAL(debug) << "Input parsed";
  BOOST_LOG_TRIVIAL(info) << "Part 1: " << part1(input);
  BOOST_LOG_TRIVIAL(info) << "Part 2: " << part2(input);
}
