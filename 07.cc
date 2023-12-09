#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

enum class Type { Five, Four, FullHouse, Three, TwoPair, Pair, High };

struct Hand {
  std::string cards;
  int bid;
  Type type;
  Type type2;
};

using Hands = std::vector<Hand>;

constexpr auto value_for_label(char label, bool jokers) {
  if (label >= '0' && label <= '9')
    return label - '0';
  else if (label == 'T')
    return 10;
  else if (label == 'J')
    return jokers ? -1 : 11;
  else if (label == 'Q')
    return 12;
  else if (label == 'K')
    return 13;
  else if (label == 'A')
    return 14;
  throw std::runtime_error{"bad card"};
}

auto count_cards(const std::string &cards) {
  auto rval = std::unordered_map<int, int>{};
  for (auto card : cards)
    ++rval[value_for_label(card, false)];
  return rval;
}

auto type_for_count(const std::unordered_map<int, int> &count, bool jokers) {
  auto joker_count = jokers && count.contains(11) ? count.at(11) : 0;

  auto highest = std::vector<int>{};
  for (const auto &pair : count)
    highest.push_back(jokers && pair.first == 11 ? 0 : pair.second);
  std::sort(highest.rbegin(), highest.rend());

  if (highest[0] + joker_count >= 5)
    return Type::Five;
  if (highest[0] + joker_count >= 4)
    return Type::Four;
  if (highest[0] + highest[1] + joker_count >= 5)
    return Type::FullHouse;
  if (highest[0] + joker_count >= 3)
    return Type::Three;
  if (highest[0] + highest[1] + joker_count >= 4)
    return Type::TwoPair;
  if (highest[0] + joker_count >= 2)
    return Type::Pair;
  return Type::High;
}

auto parse(const std::string &filename) {
  auto rval = Hands{};
  auto input_handle = std::ifstream{filename};
  if (!input_handle)
    throw std::runtime_error{"could not open file"};
  auto line = std::string{};
  auto hand = std::string{};
  auto bid = int{};
  while (input_handle >> hand >> bid) {
    auto count = count_cards(hand);
    rval.push_back({hand, bid, type_for_count(count, false), type_for_count(count, true)});
  }
  return rval;
}

auto compare_hand_rank(const Hand &a, const Hand &b, bool jokers) {
  if (!jokers && a.type != b.type)
    return a.type > b.type;
  if (jokers && a.type2 != b.type2)
    return a.type2 > b.type2;
  for (auto i = 0; i < 5; ++i) {
    auto a_value = value_for_label(a.cards[i], jokers);
    auto b_value = value_for_label(b.cards[i], jokers);
    if (a_value != b_value)
      return a_value < b_value;
  }
  throw std::runtime_error{"mismatch"};
}

auto rank_and_score(Hands hands, bool jokers) {
  std::sort(hands.begin(), hands.end(), [jokers](Hand &a, Hand &b) {
    return compare_hand_rank(a, b, jokers);
  });
  auto rval = 0;
  for (auto rank = size_t{}; rank < hands.size(); ++rank)
    rval += (rank + 1) * hands[rank].bid;
  return rval;
}

} // namespace

auto main() -> int {
  auto input = parse("input/07.txt");
  std::cout << "Part 1: " << rank_and_score(input, false) << '\n';           // 248559379
  std::cout << "Part 2: " << rank_and_score(std::move(input), true) << '\n'; // 249631254
}
