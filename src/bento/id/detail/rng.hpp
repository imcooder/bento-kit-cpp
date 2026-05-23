#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

namespace bento::id::detail {

inline std::uint64_t unixMillis() {
  using clock = std::chrono::system_clock;
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now().time_since_epoch()).count();
  if (ms < 0) {
    return 0;
  }
  return static_cast<std::uint64_t>(ms);
}

inline unsigned mixSeed(unsigned a, unsigned b) {
  return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2));
}

/// Seed that works when `std::random_device` is weak or throws (some mobile / wasm hosts).
inline unsigned seedThreadRng() {
  try {
    std::random_device rd;
    const unsigned a = static_cast<unsigned>(rd());
    const unsigned b = static_cast<unsigned>(rd());
    if (a != b) {
      return mixSeed(a, b);
    }
  } catch (...) {
  }
  const unsigned ms = static_cast<unsigned>(unixMillis() & 0xffffffffu);
  const unsigned tid =
      static_cast<unsigned>(std::hash<std::thread::id>{}(std::this_thread::get_id()) & 0xffffffffu);
  return mixSeed(ms, tid);
}

inline std::mt19937 &threadRng() {
  thread_local std::mt19937 rng{seedThreadRng()};
  return rng;
}

inline std::string pickRandomChars(std::string_view alphabet, std::size_t len) {
  if (alphabet.empty()) {
    throw std::invalid_argument("alphabet must not be empty");
  }
  std::uniform_int_distribution<std::size_t> dist(0, alphabet.size() - 1);
  std::string out;
  out.reserve(len);
  auto &rng = threadRng();
  for (std::size_t i = 0; i < len; ++i) {
    out.push_back(alphabet[dist(rng)]);
  }
  return out;
}

inline void fillRandomBytes(std::uint8_t *data, std::size_t len) {
  std::uniform_int_distribution<int> dist(0, 255);
  auto &rng = threadRng();
  for (std::size_t i = 0; i < len; ++i) {
    data[i] = static_cast<std::uint8_t>(dist(rng));
  }
}

} // namespace bento::id::detail
