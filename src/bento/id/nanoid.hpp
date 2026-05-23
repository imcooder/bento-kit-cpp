#pragma once

#include <bento/id/detail/rng.hpp>

#include <string>
#include <string_view>

namespace bento::id {

inline constexpr std::size_t kDefaultNanoidLen = 21;

inline std::string nanoid();
inline std::string nanoidWithLen(std::size_t len);
inline std::string nanoidWithAlphabet(std::size_t len, std::string_view alphabet);

inline std::string nanoid() {
  return nanoidWithLen(kDefaultNanoidLen);
}

inline std::string nanoidWithLen(std::size_t len) {
  static constexpr char kUrlSafeAlphabet[] = "_-0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  return detail::pickRandomChars(kUrlSafeAlphabet, len);
}

inline std::string nanoidWithAlphabet(std::size_t len, std::string_view alphabet) {
  return detail::pickRandomChars(alphabet, len);
}

} // namespace bento::id
