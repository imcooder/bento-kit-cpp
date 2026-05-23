#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace bento::mask {

inline std::string maskMiddle(std::string_view s, std::size_t prefix, std::size_t suffix);
inline std::string maskSecret(std::optional<std::string_view> value, std::size_t keepChars = 3);

#ifdef _WIN32
inline std::wstring maskMiddle(std::wstring_view s, std::size_t prefix, std::size_t suffix);
inline std::wstring maskSecret(std::optional<std::wstring_view> value, std::size_t keepChars = 3);
#endif

namespace detail {

inline constexpr char kMaskChar = '*';

inline std::string starString(std::size_t count) {
  return std::string(count, kMaskChar);
}

inline bool isAsciiDigit(char ch) {
  return ch >= '0' && ch <= '9';
}

inline std::size_t utf8CharByteLength(std::string_view s, std::size_t index) {
  if (index >= s.size()) {
    return 0;
  }
  const auto b0 = static_cast<unsigned char>(s[index]);
  if (b0 < 0x80) {
    return 1;
  }
  if ((b0 & 0xE0) == 0xC0 && index + 1 < s.size()) {
    const auto b1 = static_cast<unsigned char>(s[index + 1]);
    if ((b1 & 0xC0) == 0x80) {
      return 2;
    }
  }
  if ((b0 & 0xF0) == 0xE0 && index + 2 < s.size()) {
    const auto b1 = static_cast<unsigned char>(s[index + 1]);
    const auto b2 = static_cast<unsigned char>(s[index + 2]);
    if ((b1 & 0xC0) == 0x80 && (b2 & 0xC0) == 0x80) {
      return 3;
    }
  }
  if ((b0 & 0xF8) == 0xF0 && index + 3 < s.size()) {
    const auto b1 = static_cast<unsigned char>(s[index + 1]);
    const auto b2 = static_cast<unsigned char>(s[index + 2]);
    const auto b3 = static_cast<unsigned char>(s[index + 3]);
    if ((b1 & 0xC0) == 0x80 && (b2 & 0xC0) == 0x80 && (b3 & 0xC0) == 0x80) {
      return 4;
    }
  }
  return 1;
}

inline std::vector<std::size_t> utf8CharStarts(std::string_view s) {
  std::vector<std::size_t> starts;
  starts.reserve(s.size() / 2 + 2);
  for (std::size_t i = 0; i < s.size();) {
    starts.push_back(i);
    i += utf8CharByteLength(s, i);
  }
  starts.push_back(s.size());
  return starts;
}

inline std::size_t utf8CharCount(std::string_view s) {
  std::size_t count = 0;
  for (std::size_t i = 0; i < s.size();) {
    ++count;
    i += utf8CharByteLength(s, i);
  }
  return count;
}

#ifdef _WIN32

inline constexpr wchar_t kMaskWChar = L'*';

inline std::wstring starWString(std::size_t count) {
  return std::wstring(count, kMaskWChar);
}

inline bool isWideDigit(wchar_t ch) {
  return ch >= L'0' && ch <= L'9';
}

/// UTF-16 code-unit length for one Unicode scalar on Windows (`wchar_t` is UTF-16).
inline std::size_t utf16CharUnitLength(std::wstring_view s, std::size_t index) {
  if (index >= s.size()) {
    return 0;
  }
  const wchar_t w0 = s[index];
  if (w0 >= 0xD800 && w0 <= 0xDBFF && index + 1 < s.size()) {
    const wchar_t w1 = s[index + 1];
    if (w1 >= 0xDC00 && w1 <= 0xDFFF) {
      return 2;
    }
  }
  return 1;
}

inline std::vector<std::size_t> utf16CharStarts(std::wstring_view s) {
  std::vector<std::size_t> starts;
  starts.reserve(s.size() / 2 + 2);
  for (std::size_t i = 0; i < s.size();) {
    starts.push_back(i);
    i += utf16CharUnitLength(s, i);
  }
  starts.push_back(s.size());
  return starts;
}

inline std::size_t utf16CharCount(std::wstring_view s) {
  std::size_t count = 0;
  for (std::size_t i = 0; i < s.size();) {
    ++count;
    i += utf16CharUnitLength(s, i);
  }
  return count;
}

#endif // _WIN32

} // namespace detail

/// Mask an 11-digit mobile number: `13812345678` -> `138****5678`.
inline std::string maskPhone(std::string_view phone) {
  if (phone.size() == 11) {
    bool allDigits = true;
    for (char ch : phone) {
      if (!detail::isAsciiDigit(ch)) {
        allDigits = false;
        break;
      }
    }
    if (allDigits) {
      std::string out;
      out.reserve(11);
      out.append(phone.substr(0, 3));
      out.append("****");
      out.append(phone.substr(7));
      return out;
    }
  }
  return maskMiddle(phone, 1, 1);
}

/// Mask an email address: `alice@example.com` -> `a***@example.com`.
inline std::string maskEmail(std::string_view email) {
  const std::size_t at = email.find('@');
  if (at == std::string_view::npos) {
    return maskMiddle(email, 1, 0);
  }
  const std::string_view local = email.substr(0, at);
  if (local.empty()) {
    return std::string(email);
  }
  std::string out;
  out.reserve(email.size() + 4);
  out.append(local.substr(0, detail::utf8CharByteLength(local, 0)));
  out.append("***");
  out.append(email.substr(at));
  return out;
}

/// Mask a national ID card number (15 or 18 characters).
inline std::string maskIdCard(std::string_view id) {
  const std::size_t len = detail::utf8CharCount(id);
  if (len != 15 && len != 18) {
    return maskMiddle(id, 1, 1);
  }
  return maskMiddle(id, 6, 4);
}

/// Mask a bank card number, keeping the first 4 and last 4 characters.
inline std::string maskBankCard(std::string_view card) {
  if (detail::utf8CharCount(card) < 9) {
    return maskMiddle(card, 1, 1);
  }
  return maskMiddle(card, 4, 4);
}

/// Mask an API token / secret (first 4 + last 4, or all stars if short).
inline std::string maskToken(std::string_view token) {
  const std::size_t len = detail::utf8CharCount(token);
  if (len < 9) {
    return detail::starString(len);
  }
  return maskMiddle(token, 4, 4);
}

/// Mask a CJK personal name (1 char unchanged, 2 -> first+*, 3+ -> first+stars+last).
inline std::string maskName(std::string_view name) {
  const auto starts = detail::utf8CharStarts(name);
  const std::size_t charCount = starts.size() - 1;
  switch (charCount) {
  case 0:
  case 1:
    return std::string(name);
  case 2: {
    std::string out;
    out.append(name.substr(0, detail::utf8CharByteLength(name, 0)));
    out.push_back(detail::kMaskChar);
    return out;
  }
  default: {
    std::string out;
    out.append(name.substr(0, detail::utf8CharByteLength(name, 0)));
    out.append(detail::starString(charCount - 2));
    out.append(name.substr(starts[charCount - 1]));
    return out;
  }
  }
}

/// Generic credential masking with a fixed `***` middle segment.
inline std::string maskSecret(std::optional<std::string_view> value, std::size_t keepChars) {
  if (!value.has_value()) {
    return "<none>";
  }
  if (value->empty()) {
    return "<empty>";
  }
  const auto starts = detail::utf8CharStarts(*value);
  const std::size_t len = starts.size() - 1;
  if (len <= keepChars * 2) {
    return std::string(*value);
  }
  std::string out;
  out.reserve(value->size() + 3);
  out.append(value->substr(0, starts[keepChars]));
  out.append("***");
  out.append(value->substr(starts[len - keepChars]));
  return out;
}

/// Keep `prefix` / `suffix` Unicode chars; star the middle (one star per char).
inline std::string maskMiddle(std::string_view s, std::size_t prefix, std::size_t suffix) {
  const auto starts = detail::utf8CharStarts(s);
  const std::size_t total = starts.size() - 1;
  if (total <= prefix + suffix) {
    return detail::starString(total);
  }
  std::string out;
  out.reserve(s.size());
  out.append(s.substr(0, starts[prefix]));
  out.append(detail::starString(total - prefix - suffix));
  out.append(s.substr(starts[total - suffix]));
  return out;
}

#ifdef _WIN32

inline std::wstring maskPhone(std::wstring_view phone) {
  if (phone.size() == 11) {
    bool allDigits = true;
    for (wchar_t ch : phone) {
      if (!detail::isWideDigit(ch)) {
        allDigits = false;
        break;
      }
    }
    if (allDigits) {
      std::wstring out;
      out.reserve(11);
      out.append(phone.substr(0, 3));
      out.append(L"****");
      out.append(phone.substr(7));
      return out;
    }
  }
  return maskMiddle(phone, 1, 1);
}

inline std::wstring maskEmail(std::wstring_view email) {
  const std::size_t at = email.find(L'@');
  if (at == std::wstring_view::npos) {
    return maskMiddle(email, 1, 0);
  }
  const std::wstring_view local = email.substr(0, at);
  if (local.empty()) {
    return std::wstring(email);
  }
  std::wstring out;
  out.reserve(email.size() + 4);
  out.append(local.substr(0, detail::utf16CharUnitLength(local, 0)));
  out.append(L"***");
  out.append(email.substr(at));
  return out;
}

inline std::wstring maskIdCard(std::wstring_view id) {
  const std::size_t len = detail::utf16CharCount(id);
  if (len != 15 && len != 18) {
    return maskMiddle(id, 1, 1);
  }
  return maskMiddle(id, 6, 4);
}

inline std::wstring maskBankCard(std::wstring_view card) {
  if (detail::utf16CharCount(card) < 9) {
    return maskMiddle(card, 1, 1);
  }
  return maskMiddle(card, 4, 4);
}

inline std::wstring maskToken(std::wstring_view token) {
  const std::size_t len = detail::utf16CharCount(token);
  if (len < 9) {
    return detail::starWString(len);
  }
  return maskMiddle(token, 4, 4);
}

inline std::wstring maskName(std::wstring_view name) {
  const auto starts = detail::utf16CharStarts(name);
  const std::size_t charCount = starts.size() - 1;
  switch (charCount) {
  case 0:
  case 1:
    return std::wstring(name);
  case 2: {
    std::wstring out;
    out.append(name.substr(0, detail::utf16CharUnitLength(name, 0)));
    out.push_back(detail::kMaskWChar);
    return out;
  }
  default: {
    std::wstring out;
    out.append(name.substr(0, detail::utf16CharUnitLength(name, 0)));
    out.append(detail::starWString(charCount - 2));
    out.append(name.substr(starts[charCount - 1]));
    return out;
  }
  }
}

inline std::wstring maskSecret(std::optional<std::wstring_view> value, std::size_t keepChars) {
  if (!value.has_value()) {
    return L"<none>";
  }
  if (value->empty()) {
    return L"<empty>";
  }
  const auto starts = detail::utf16CharStarts(*value);
  const std::size_t len = starts.size() - 1;
  if (len <= keepChars * 2) {
    return std::wstring(*value);
  }
  std::wstring out;
  out.reserve(value->size() + 3);
  out.append(value->substr(0, starts[keepChars]));
  out.append(L"***");
  out.append(value->substr(starts[len - keepChars]));
  return out;
}

inline std::wstring maskMiddle(std::wstring_view s, std::size_t prefix, std::size_t suffix) {
  const auto starts = detail::utf16CharStarts(s);
  const std::size_t total = starts.size() - 1;
  if (total <= prefix + suffix) {
    return detail::starWString(total);
  }
  std::wstring out;
  out.reserve(s.size());
  out.append(s.substr(0, starts[prefix]));
  out.append(detail::starWString(total - prefix - suffix));
  out.append(s.substr(starts[total - suffix]));
  return out;
}

#endif // _WIN32

} // namespace bento::mask
