#pragma once

#include <string>

namespace bento::test {

// UTF-8 CJK byte sequences for mask tests (avoid non-ASCII literals in sources).

inline std::string utf8CjkNameOne() {
  return std::string(u8"\xe5\xbc\xa0", 3);
}

inline std::string utf8CjkNameTwo() {
  return std::string(u8"\xe5\xbc\xa0\xe4\xb8\x89", 6);
}

inline std::string utf8CjkNameThree() {
  return std::string(u8"\xe5\xbc\xa0\xe4\xb8\x89\xe4\xb8\xb0", 9);
}

inline std::string utf8CjkNameFour() {
  return std::string(u8"\xe6\xac\xa7\xe9\x98\xb3\xe4\xbf\xae\xe8\xbf\x9c", 12);
}

inline std::string utf8CjkNameTwoMasked() {
  return utf8CjkNameOne() + "*";
}

inline std::string utf8CjkNameThreeMasked() {
  return std::string(u8"\xe5\xbc\xa0", 3) + "*" + std::string(u8"\xe4\xb8\xb0", 3);
}

inline std::string utf8CjkNameFourMasked() {
  return std::string(u8"\xe6\xac\xa7", 3) + "**" + std::string(u8"\xe8\xbf\x9c", 3);
}

inline std::string utf8CjkDigitsFive() {
  return std::string(u8"\xe4\xb8\x80\xe4\xba\x8c\xe4\xb8\x89\xe5\x9b\x9b\xe4\xba\x94", 15);
}

inline std::string utf8CjkDigitsEight() {
  return std::string(u8"\xe4\xb8\x80\xe4\xba\x8c\xe4\xb8\x89\xe5\x9b\x9b\xe4\xba\x94"
                     u8"\xe5\x85\xad\xe4\xb8\x83\xe5\x85\xab",
                     24);
}

inline std::string utf8CjkMaskMiddleExpected() {
  return std::string(u8"\xe4\xb8\x80", 3) + "***" + std::string(u8"\xe4\xba\x94", 3);
}

inline std::string utf8CjkMaskSecretExpected() {
  return std::string(u8"\xe4\xb8\x80\xe4\xba\x8c", 6) + "***" + std::string(u8"\xe4\xb8\x83\xe5\x85\xab", 6);
}

} // namespace bento::test
