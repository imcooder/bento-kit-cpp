#pragma once

#include <cstdint>
#include <string>

namespace bento::test {

inline std::string makeUtf8(const unsigned char *bytes, std::size_t len) {
  return std::string(reinterpret_cast<const char *>(bytes), len);
}

inline std::string utf8CjkNameOne() {
  static const unsigned char kBytes[] = {0xE5, 0xBC, 0xA0};
  return makeUtf8(kBytes, sizeof(kBytes));
}

inline std::string utf8CjkNameTwo() {
  static const unsigned char kBytes[] = {0xE5, 0xBC, 0xA0, 0xE4, 0xB8, 0x89};
  return makeUtf8(kBytes, sizeof(kBytes));
}

inline std::string utf8CjkNameThree() {
  static const unsigned char kBytes[] = {0xE5, 0xBC, 0xA0, 0xE4, 0xB8, 0x89, 0xE4, 0xB8, 0xB0};
  return makeUtf8(kBytes, sizeof(kBytes));
}

inline std::string utf8CjkNameFour() {
  static const unsigned char kBytes[] = {0xE6, 0xAC, 0xA7, 0xE9, 0x98, 0xB3, 0xE4, 0xBF, 0xAE, 0xE8, 0xBF, 0x9C};
  return makeUtf8(kBytes, sizeof(kBytes));
}

inline std::string utf8CjkNameTwoMasked() {
  return utf8CjkNameOne() + "*";
}

inline std::string utf8CjkNameThreeMasked() {
  static const unsigned char kHead[] = {0xE5, 0xBC, 0xA0};
  static const unsigned char kTail[] = {0xE4, 0xB8, 0xB0};
  return makeUtf8(kHead, sizeof(kHead)) + "*" + makeUtf8(kTail, sizeof(kTail));
}

inline std::string utf8CjkNameFourMasked() {
  static const unsigned char kHead[] = {0xE6, 0xAC, 0xA7};
  static const unsigned char kTail[] = {0xE8, 0xBF, 0x9C};
  return makeUtf8(kHead, sizeof(kHead)) + "**" + makeUtf8(kTail, sizeof(kTail));
}

inline std::string utf8CjkDigitsFive() {
  static const unsigned char kBytes[] = {0xE4, 0xB8, 0x80, 0xE4, 0xBA, 0x8C, 0xE4, 0xB8, 0x89, 0xE5, 0x9B, 0x9B, 0xE4, 0xBA, 0x94};
  return makeUtf8(kBytes, sizeof(kBytes));
}

inline std::string utf8CjkDigitsEight() {
  static const unsigned char kBytes[] = {0xE4, 0xB8, 0x80, 0xE4, 0xBA, 0x8C, 0xE4, 0xB8, 0x89, 0xE5, 0x9B, 0x9B, 0xE4, 0xBA, 0x94, 0xE5, 0x85, 0xAD, 0xE4, 0xB8, 0x83, 0xE5, 0x85, 0xAB};
  return makeUtf8(kBytes, sizeof(kBytes));
}

inline std::string utf8CjkMaskMiddleExpected() {
  static const unsigned char kHead[] = {0xE4, 0xB8, 0x80};
  static const unsigned char kTail[] = {0xE4, 0xBA, 0x94};
  return makeUtf8(kHead, sizeof(kHead)) + "***" + makeUtf8(kTail, sizeof(kTail));
}

inline std::string utf8CjkMaskSecretExpected() {
  static const unsigned char kHead[] = {0xE4, 0xB8, 0x80, 0xE4, 0xBA, 0x8C};
  static const unsigned char kTail[] = {0xE4, 0xB8, 0x83, 0xE5, 0x85, 0xAB};
  return makeUtf8(kHead, sizeof(kHead)) + "***" + makeUtf8(kTail, sizeof(kTail));
}

#include <bento/platform.hpp>

#if BENTO_HAS_WSTRING_MASK

inline std::wstring wideFromScalars(std::initializer_list<char32_t> scalars) {
  std::wstring out;
  out.reserve(scalars.size());
  for (char32_t cp : scalars) {
    if (cp <= 0xFFFF) {
      out.push_back(static_cast<wchar_t>(cp));
    } else {
      cp -= 0x10000;
      out.push_back(static_cast<wchar_t>(0xD800 + (cp >> 10)));
      out.push_back(static_cast<wchar_t>(0xDC00 + (cp & 0x3FF)));
    }
  }
  return out;
}

inline std::wstring utf16CjkDigitsEightWide() {
  return wideFromScalars({0x4E00, 0x4E8C, 0x4E09, 0x56DB, 0x4E94, 0x516D, 0x4E03, 0x516B});
}

inline std::wstring utf16CjkMaskSecretExpectedWide() {
  return wideFromScalars({0x4E00, 0x4E8C}) + L"***" + wideFromScalars({0x4E03, 0x516B});
}

inline std::wstring utf16CjkNameTwoWide() {
  return wideFromScalars({0x5F20, 0x4E09});
}

inline std::wstring utf16CjkNameTwoMaskedWide() {
  return wideFromScalars({0x5F20}) + L"*";
}

inline std::wstring utf16CjkDigitsFiveWide() {
  return wideFromScalars({0x4E00, 0x4E8C, 0x4E09, 0x56DB, 0x4E94});
}

inline std::wstring utf16CjkMaskMiddleExpectedWide() {
  return wideFromScalars({0x4E00}) + L"***" + wideFromScalars({0x4E94});
}

#endif // BENTO_HAS_WSTRING_MASK

} // namespace bento::test
