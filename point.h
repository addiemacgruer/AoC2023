#pragma once

#include <iostream>
#include <unordered_map>

struct Point {
  int x;
  int y;

  auto operator+=(const Point &other) {
    x += other.x;
    y += other.y;
    return *this;
  }
};

inline auto operator<(const Point &a, const Point &b) {
  return a.y == b.y ? a.x < b.x : a.y < b.y;
}

inline auto operator==(const Point &a, const Point &b) {
  return a.x == b.x && a.y == b.y;
}

inline auto operator!=(const Point &a, const Point &b) {
  return a.x != b.x || a.y != b.y;
}

inline auto operator+(Point a, const Point &b) {
  return a += b;
}

inline auto operator-(const Point &a, const Point &b) {
  return Point{a.x - b.x, a.y - b.y};
}

inline auto operator<<(std::ostream &oh, const Point &p) -> std::ostream & {
  return oh << '{' << p.x << ',' << p.y << '}';
}

inline auto manhattan(const Point &a, const Point &b) {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

namespace P {

static auto up = Point{0, -1};
static auto down = Point{0, 1};
static auto left = Point{-1, 0};
static auto right = Point{1, 0};

} // namespace P

template <>
struct std::hash<Point> {
  std::size_t operator()(const Point &p) const noexcept {
    return p.x ^ (p.y << (sizeof(int) * 4));
  }
};
