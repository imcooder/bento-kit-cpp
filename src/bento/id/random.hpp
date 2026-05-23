#pragma once

#include <bento/id/detail/rng.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>

namespace bento::id {

/// Random integer in the half-open range `[min, max)`.
///
/// Not cryptographically secure - suitable for non-security IDs only.
inline std::int64_t randomInt(std::int64_t min, std::int64_t max) {
  if (min >= max) {
    throw std::invalid_argument("randomInt: min must be less than max");
  }
  std::uniform_int_distribution<std::int64_t> dist(min, max - 1);
  return dist(detail::threadRng());
}

/// Random string using a fixed 36-character keyboard-order alphabet.
inline std::string randomString(std::size_t len) {
  static constexpr char kAlphabet[] = "0123456789qwertyuioplkjhgfdsazxcvbnm";
  return detail::pickRandomChars(kAlphabet, len);
}

namespace detail {

inline std::string generateBase36String(std::size_t len) {
  static constexpr char kBase36Alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  return pickRandomChars(kBase36Alphabet, len);
}

} // namespace detail

} // namespace bento::id
