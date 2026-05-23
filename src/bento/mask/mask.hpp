#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace bento::mask {

inline std::string maskMiddle(std::string_view s, std::size_t prefix, std::size_t suffix);
inline std::string maskSecret(std::optional<std::string_view> value, std::size_t keepChars = 3);

namespace detail {

inline constexpr char kMaskChar = '*';

inline std::string starString(std::size_t count) {
  return std::string(count, kMaskChar);
}

inline void appendUtf8(std::string &out, char32_t cp) {
  if (cp <= 0x7F) {
    out.push_back(static_cast<char>(cp));
  } else if (cp <= 0x7FF) {
    out.push_back(static_cast<char>(0xC0 | ((cp >> 6) & 0x1F)));
    out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
  } else if (cp <= 0xFFFF) {
    out.push_back(static_cast<char>(0xE0 | ((cp >> 12) & 0x0F)));
    out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
    out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
  } else {
    out.push_back(static_cast<char>(0xF0 | ((cp >> 18) & 0x07)));
    out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
    out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
    out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
  }
}

inline std::vector<char32_t> utf8ToChars(std::string_view s) {
  std::vector<char32_t> chars;
  chars.reserve(s.size());
  for (std::size_t i = 0; i < s.size();) {
    const auto b0 = static_cast<unsigned char>(s[i]);
    if (b0 < 0x80) {
      chars.push_back(b0);
      ++i;
      continue;
    }
    if ((b0 & 0xE0) == 0xC0 && i + 1 < s.size()) {
      const auto b1 = static_cast<unsigned char>(s[i + 1]);
      if ((b1 & 0xC0) == 0x80) {
        chars.push_back(((b0 & 0x1F) << 6) | (b1 & 0x3F));
        i += 2;
        continue;
      }
    }
    if ((b0 & 0xF0) == 0xE0 && i + 2 < s.size()) {
      const auto b1 = static_cast<unsigned char>(s[i + 1]);
      const auto b2 = static_cast<unsigned char>(s[i + 2]);
      if ((b1 & 0xC0) == 0x80 && (b2 & 0xC0) == 0x80) {
        chars.push_back(((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F));
        i += 3;
        continue;
      }
    }
    if ((b0 & 0xF8) == 0xF0 && i + 3 < s.size()) {
      const auto b1 = static_cast<unsigned char>(s[i + 1]);
      const auto b2 = static_cast<unsigned char>(s[i + 2]);
      const auto b3 = static_cast<unsigned char>(s[i + 3]);
      if ((b1 & 0xC0) == 0x80 && (b2 & 0xC0) == 0x80 && (b3 & 0xC0) == 0x80) {
        chars.push_back(((b0 & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F));
        i += 4;
        continue;
      }
    }
    chars.push_back(U'\uFFFD');
    ++i;
  }
  return chars;
}

inline std::string charsToUtf8(const std::vector<char32_t> &chars) {
  std::string out;
  out.reserve(chars.size() * 3);
  for (char32_t cp : chars) {
    appendUtf8(out, cp);
  }
  return out;
}

inline std::string charsRangeToUtf8(const std::vector<char32_t> &chars, std::size_t begin, std::size_t end) {
  std::string out;
  out.reserve((end - begin) * 3);
  for (std::size_t i = begin; i < end; ++i) {
    appendUtf8(out, chars[i]);
  }
  return out;
}

inline bool isAsciiDigit(char ch) {
  return ch >= '0' && ch <= '9';
}

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
  const std::vector<char32_t> chars = detail::utf8ToChars(local);
  std::string out;
  out.reserve(email.size() + 4);
  detail::appendUtf8(out, chars.front());
  out.append("***");
  out.append(email.substr(at));
  return out;
}

/// Mask a national ID card number (15 or 18 characters).
inline std::string maskIdCard(std::string_view id) {
  const auto chars = detail::utf8ToChars(id);
  const std::size_t len = chars.size();
  if (len != 15 && len != 18) {
    return maskMiddle(id, 1, 1);
  }
  return maskMiddle(id, 6, 4);
}

/// Mask a bank card number, keeping the first 4 and last 4 characters.
inline std::string maskBankCard(std::string_view card) {
  if (detail::utf8ToChars(card).size() < 9) {
    return maskMiddle(card, 1, 1);
  }
  return maskMiddle(card, 4, 4);
}

/// Mask an API token / secret (first 4 + last 4, or all stars if short).
inline std::string maskToken(std::string_view token) {
  const std::size_t len = detail::utf8ToChars(token).size();
  if (len < 9) {
    return detail::starString(len);
  }
  return maskMiddle(token, 4, 4);
}

/// Mask a CJK personal name (1 char unchanged, 2 -> first+*, 3+ -> first+stars+last).
inline std::string maskName(std::string_view name) {
  const auto chars = detail::utf8ToChars(name);
  switch (chars.size()) {
  case 0:
  case 1:
    return std::string(name);
  case 2: {
    std::string out;
    detail::appendUtf8(out, chars[0]);
    out.push_back(detail::kMaskChar);
    return out;
  }
  default: {
    std::string out;
    detail::appendUtf8(out, chars.front());
    out.append(detail::starString(chars.size() - 2));
    detail::appendUtf8(out, chars.back());
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
  const auto chars = detail::utf8ToChars(*value);
  const std::size_t len = chars.size();
  if (len <= keepChars * 2) {
    return std::string(*value);
  }
  std::string out;
  out.reserve(len * 3 + 3);
  out.append(detail::charsRangeToUtf8(chars, 0, keepChars));
  out.append("***");
  out.append(detail::charsRangeToUtf8(chars, len - keepChars, len));
  return out;
}

/// Keep `prefix` / `suffix` Unicode chars; star the middle (one star per char).
inline std::string maskMiddle(std::string_view s, std::size_t prefix, std::size_t suffix) {
  const auto chars = detail::utf8ToChars(s);
  const std::size_t total = chars.size();
  if (total <= prefix + suffix) {
    return detail::starString(total);
  }
  std::string out;
  out.reserve(total * 3);
  out.append(detail::charsRangeToUtf8(chars, 0, prefix));
  out.append(detail::starString(total - prefix - suffix));
  out.append(detail::charsRangeToUtf8(chars, total - suffix, total));
  return out;
}

} // namespace bento::mask
